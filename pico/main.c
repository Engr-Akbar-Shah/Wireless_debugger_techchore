#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "tusb.h"
#include "bridge_engine.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#define UART_RX_RING 32768U
#define UART_TX_RING 8192U
_Static_assert(PC_DATA_WINDOW*(PC_WIRE_MAX+12U)+512U<UART_TX_RING,"Control/data window must fit Pico UART queue");
static uint8_t uart_rx[UART_RX_RING],uart_tx[UART_TX_RING],logs[4096];
static volatile uint32_t rx_head,rx_tail,rx_overflow,uart_errors;
static volatile uint32_t uart_fe,uart_pe,uart_be,uart_oe;
static uint32_t tx_head,tx_tail,log_head,log_tail;
static pc_parser_t parser;
static bool packet_pending;
static void uart_interrupt(void)
{
  while(uart_is_readable(uart0)) {
    uint32_t word=uart_get_hw(uart0)->dr;
    if(word&0xf00U)uart_errors++;
    if(word&(1U<<8))uart_fe++;
    if(word&(1U<<9))uart_pe++;
    if(word&(1U<<10))uart_be++;
    if(word&(1U<<11))uart_oe++;
    if(rx_head-rx_tail<UART_RX_RING) {uart_rx[rx_head%UART_RX_RING]=(uint8_t)word;__dmb();rx_head++;}
    else rx_overflow++;
  }
  uart_get_hw(uart0)->rsr=0;
}
bool bridge_port_send(uint8_t type,uint16_t id,const uint8_t *data,unsigned n)
{
  uint8_t wire[PC_WIRE_MAX];unsigned count=pc_encode(wire,type,id,data,n);
  if(!count || count>UART_TX_RING-(tx_head-tx_tail))return false;
  for(unsigned i=0;i<count;i++)uart_tx[(tx_head+i)%UART_TX_RING]=wire[i];
  tx_head+=count;return true;
}
unsigned bridge_usb_read(uint8_t *data,unsigned n) { return tud_cdc_n_read(0,data,n); }
unsigned bridge_usb_write(const uint8_t *data,unsigned n)
{
  /* TinyUSB0.18 makes its TX FIFO overwritable when DTR is released.
   * DTR=0 is normal here: it releases the target's reset. The only payload
   * writer runs in this task; USB can consume, but cannot reduce free space
   * between this check and write. Keep any unaccepted bytes in bridge.output. */
  unsigned available=tud_cdc_n_write_available(0);
  if(n>available)n=available;
  unsigned count=n ? tud_cdc_n_write(0,data,n) : 0;
  /* Also retry flushing an already full FIFO after endpoint backpressure. */
  tud_cdc_n_write_flush(0);
  return count;
}
void bridge_usb_discard_output(void) { tud_cdc_n_write_clear(0); }
void bridge_usb_flush(void) { tud_cdc_n_read_flush(0);tud_cdc_n_write_clear(0); }
void bridge_log(const char *format,...)
{
  char line[320];va_list args;va_start(args,format);int n=vsnprintf(line,sizeof(line),format,args);va_end(args);
  if(n<0)return;
  if(n>=(int)sizeof(line))n=sizeof(line)-1;
  for(int i=0;i<n;i++) {if(log_head-log_tail>=sizeof(logs))log_tail++;logs[log_head++%sizeof(logs)]=(uint8_t)line[i];}
}
void tud_cdc_line_coding_cb(uint8_t itf,cdc_line_coding_t const *coding)
{ if(itf==0)bridge_line_changed(coding->bit_rate,coding->data_bits,coding->parity,coding->stop_bits); }
void tud_cdc_line_state_cb(uint8_t itf,bool dtr,bool rts)
{ if(itf==0)bridge_usb_lines(dtr,rts); }
void tud_mount_cb(void) { bridge_mounted(true); }
void tud_umount_cb(void) { bridge_mounted(false); }
static void service_status(void)
{
  static char command[32];static unsigned used;
  while(tud_cdc_n_available(1)) {
    int c=tud_cdc_n_read_char(1);if(c=='\r')continue;
    if(c=='\n') {
      command[used]=0;used=0;
      if(!strcmp(command,"STATUS")) {
        bridge_status();
        bridge_log("UART fe=%lu pe=%lu be=%lu oe=%lu overflow=%lu\n",(unsigned long)uart_fe,
          (unsigned long)uart_pe,(unsigned long)uart_be,(unsigned long)uart_oe,(unsigned long)rx_overflow);
      }
      else if(!strcmp(command,"BOOTSEL"))reset_usb_boot(0,0);
      else bridge_log("Commands: STATUS; data on the other COM port\n");
    } else if(c>=0 && used<sizeof(command)-1) command[used++]=(char)c;
  }
  while(log_tail!=log_head && tud_cdc_n_write_available(1)) {
    uint8_t byte=logs[log_tail%sizeof(logs)];if(!tud_cdc_n_write(1,&byte,1))break;log_tail++;
  }
  tud_cdc_n_write_flush(1);
}
int main(void)
{
  gpio_init(PICO_DEFAULT_LED_PIN);gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);
  uart_init(uart0,2000000);gpio_set_function(0,GPIO_FUNC_UART);gpio_set_function(1,GPIO_FUNC_UART);
  uart_set_format(uart0,8,1,UART_PARITY_NONE);uart_set_hw_flow(uart0,false,false);uart_set_fifo_enabled(uart0,true);
  irq_set_exclusive_handler(UART0_IRQ,uart_interrupt);
  /* At2M the32-byte RX FIFO spans160us. UART must preempt USB service;
   * the RX ISR only copies bytes/counters and never calls TinyUSB. */
  irq_set_priority(UART0_IRQ,PICO_HIGHEST_IRQ_PRIORITY);
  irq_set_enabled(UART0_IRQ,true);uart_set_irq_enables(uart0,true,false);
  bridge_init();tusb_init();bridge_log("Wireless UART Pico3.12.1 auto-boot 8E1, auto-reset DTR/RTS: COBS3,24-frame window,bounded USB output; GP0 TX GP1 RX; fixed2M\n");
  uint32_t old_overflow=0,old_uart_errors=0;
  while(1) {
    tud_task();bridge.now=to_ms_since_boot(get_absolute_time());
    while(tx_tail!=tx_head && uart_is_writable(uart0))uart_get_hw(uart0)->dr=uart_tx[tx_tail++%UART_TX_RING];
    if(rx_overflow!=old_overflow || uart_errors!=old_uart_errors) {
      bridge_log("UART FAULT overflow=%lu errors=%lu; disconnect/reconnect USB for investigation\n",(unsigned long)rx_overflow,(unsigned long)uart_errors);
      old_overflow=rx_overflow;old_uart_errors=uart_errors;bridge_transport_fault("UART error while peer restarted");
    }
    if(packet_pending) packet_pending=!bridge_packet(parser.type,parser.id,parser.raw+6,parser.length);
    unsigned budget=2048;
    while(!packet_pending && budget-- && rx_tail!=rx_head) {
      uint8_t byte=uart_rx[rx_tail++%UART_RX_RING];int result=pc_feed(&parser,byte);
      if(result<0) {bridge_log("UART CRC/frame error; re-synchronize session\n");bridge_transport_fault("UART frame CRC");}
      if(result==1)packet_pending=!bridge_packet(parser.type,parser.id,parser.raw+6,parser.length);
    }
    bridge_step(to_ms_since_boot(get_absolute_time()));service_status();
    gpio_put(PICO_DEFAULT_LED_PIN,bridge_ready() ? 1 : ((bridge.now/(bridge.fault ? 100U : 500U))&1U));
    tight_loop_contents();
  }
}
