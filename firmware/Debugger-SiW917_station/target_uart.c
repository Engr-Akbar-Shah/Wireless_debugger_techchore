#include "target_uart.h"

#include "USART.h"
#include "clock_update.h"
#include "cmsis_os2.h"
#include "rsi_udma.h"
#include "rsi_egpio.h"
#include "rsi_rom_egpio.h"
#include "../queued_uart_tx.h"
#include "../wireless_uart_rate.h"
#include "../pc_link.h"
#define BRIDGE_DIAGNOSTIC_SOURCE_IMPLEMENTATION
#include "../diagnostic_source.h"
static volatile bool target_reconfiguring;
static uint32_t target_nominal_baud=WIRELESS_UART_NOMINAL_BAUD;
static uint32_t target_driver_baud=WIRELESS_UART_DRIVER_BAUD;
static bool target_even_parity;
static bool target_baud_valid=true;

extern ARM_DRIVER_USART Driver_USART0;

static target_uart_rx_callback_t receive_callback;
extern volatile uint32_t target_uart_received_bytes;
static volatile bool boot_sync_pending;
static volatile uint32_t boot_sync_discarded;
volatile uint32_t target_uart_boot_prefix_discarded;
void target_uart_prepare_bootloader(bool enabled)
{
  boot_sync_pending = enabled && target_even_parity;
  boot_sync_discarded = 0U;
}
volatile uint32_t target_uart_rx_hash = 2166136261U;
volatile uint32_t target_uart_tx_hash = 2166136261U;
#define TARGET_RX_BLOCK_SIZE 256U
static uint8_t received_block[2][TARGET_RX_BLOCK_SIZE];
static volatile uint32_t rx_active, rx_published;
static volatile uint32_t rx_activity_tick;
extern RSI_UDMA_DESC_T UDMA0_Table[];
extern UDMA_Channel_Info udma0_chnl_info[];
static void target_rx_dma_event(uint32_t event, uint32_t channel);
static volatile bool rx_pingpong_running;
static RSI_UDMA_CHA_CONFIG_DATA_T rx_dma_template;
volatile uint32_t target_uart_rx_dma_stalls, target_uart_rx_dma_blocks;
extern volatile uint32_t target_uart_rx_errors;
static volatile RSI_UDMA_DESC_T *rx_descriptor(uint32_t slot)
{
  return &UDMA0_Table[24U + slot * 32U];
}
static void publish_received(const uint8_t *data, uint32_t length)
{
  if(bridge_diagnostic_mode)return; /* Explicit source isolation excludes physical UART DATA. */
  target_uart_received_bytes += length;
  for (uint32_t i = 0; i < length; ++i)
    target_uart_rx_hash = (target_uart_rx_hash ^ data[i]) * 16777619U;
  uint32_t first = 0U;
  while (boot_sync_pending && first < length) {
    if (data[first] != 0x79U && data[first] != 0x1FU && boot_sync_discarded < 512U) {
      boot_sync_discarded++; target_uart_boot_prefix_discarded++; first++;
    } else boot_sync_pending = false;
  }
  if (first < length) receive_callback(data + first, length - first);
}
/* Hardware switches to the alternate256-byte descriptor immediately. The
 * CPU has a whole block (~2.8ms), rather than only the16-byte FIFO (~174us),
 * to publish/rearm the completed buffer. No driver Receive reinitialization
 * occurs in a completion interrupt. */
static void rx_arm_descriptor(uint32_t slot)
{
  volatile RSI_UDMA_DESC_T *d = rx_descriptor(slot);
  d->pSrcEndAddr = (void *)&UART0->RBR;
  d->pDstEndAddr = received_block[slot] + TARGET_RX_BLOCK_SIZE - 1U;
  RSI_UDMA_CHA_CONFIG_DATA_T control = rx_dma_template;
  control.totalNumOfDMATrans = TARGET_RX_BLOCK_SIZE - 1U;
  control.transferType = UDMA_MODE_PINGPONG;
  __DMB();
  d->vsUDMAChaConfigData1 = control;
}
static int32_t target_rx_start(void)
{
  uint32_t irq_state = __get_PRIMASK();
  __disable_irq();
  rx_pingpong_running = false;
  int32_t status = Driver_USART0.Receive(received_block[0], TARGET_RX_BLOCK_SIZE);
  if (status == ARM_DRIVER_OK) {
    UDMA0->CHNL_ENABLE_CLR = 1UL << 24;
    rx_dma_template = rx_descriptor(0)->vsUDMAChaConfigData1;
    /* Keep the SDK IRQ dispatcher, replacing only this channel's callback. */
    udma0_chnl_info[24].cb_event = target_rx_dma_event;
    rx_active = rx_published = 0U;
    rx_arm_descriptor(0); rx_arm_descriptor(1);
    UDMA0->CHNL_PRI_ALT_CLR = 1UL << 24;
    UDMA0->UDMA_DONE_STATUS_REG = 1UL << 24;
    rx_pingpong_running = true;
    __DMB();
    UDMA0->CHNL_ENABLE_SET = 1UL << 24;
  }
  __set_PRIMASK(irq_state);
  return status;
}
/* Called by the unchanged SDK dispatcher solely for RX channel24. */
static void target_uart_dma_rx_irq(uint32_t pending)
{
  if (!rx_pingpong_running || !pending) return;
  if ((UART0->LSR & 0x1EU) != 0U) target_uart_rx_errors++;
  for (uint32_t i = 0; i < 2U; ++i) {
    uint32_t slot = rx_active;
    if (rx_descriptor(slot)->vsUDMAChaConfigData1.transferType != 0U) break;
    if (rx_descriptor(slot ^ 1U)->vsUDMAChaConfigData1.transferType == 0U)
      target_uart_rx_dma_stalls++;
    uint32_t published = rx_published;
    publish_received(received_block[slot] + published, TARGET_RX_BLOCK_SIZE - published);
    rx_arm_descriptor(slot);
    rx_active = slot ^ 1U; rx_published = 0U;
    rx_activity_tick = osKernelGetTickCount();
    target_uart_rx_dma_blocks++;
  }
  __DMB();
  UDMA0->CHNL_ENABLE_SET = 1UL << 24;
}
static void target_rx_dma_event(uint32_t event, uint32_t channel)
{
  (void)channel;
  if (event == UDMA_EVENT_XFER_DONE) target_uart_dma_rx_irq(1U);
  else if (event == UDMA_EVENT_ERROR) target_uart_rx_errors++;
}
static uint32_t dma_received_count(void)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = rx_descriptor(rx_active)->vsUDMAChaConfigData1;
  if (control.transferType == 0U) return TARGET_RX_BLOCK_SIZE;
  uint32_t remaining = control.totalNumOfDMATrans + 1U;
  return remaining <= TARGET_RX_BLOCK_SIZE ? TARGET_RX_BLOCK_SIZE - remaining : 0U;
}
bool target_uart_rx_idle(void)
{
  uint32_t idle_ticks = (osKernelGetTickFreq() * 4U + 999U) / 1000U;
  if(bridge_diagnostic_mode)return (uint32_t)(osKernelGetTickCount()-rx_activity_tick)>=idle_ticks;
  return (uint32_t)(osKernelGetTickCount() - rx_activity_tick) >= idle_ticks
      && dma_received_count() <= rx_published;
}
static void diagnostic_publish(const uint8_t *data,uint32_t length)
{
  target_uart_received_bytes+=length;
  for(uint32_t i=0;i<length;i++)target_uart_rx_hash=(target_uart_rx_hash^data[i])*16777619U;
  rx_activity_tick=osKernelGetTickCount();receive_callback(data,length);
}
static void target_rx_idle_task(void *argument)
{
  (void)argument;
  uint32_t previous = 0, changed = osKernelGetTickCount();
  uint32_t idle_ticks = (osKernelGetTickFreq() * 2U + 999U) / 1000U;
  while (1) {
    if(target_reconfiguring) { osDelay(1U);continue; }
    uint32_t active = rx_active, count = dma_received_count();
    uint32_t now = osKernelGetTickCount();
    diagnostic_source_service(now,osKernelGetTickFreq(),diagnostic_publish);
    if (count != previous) { previous = count; changed = now; rx_activity_tick = now; }
    if (count > 0U && count < TARGET_RX_BLOCK_SIZE && now - changed >= idle_ticks) {
      uint32_t irq_state = __get_PRIMASK();
      __disable_irq();
      if (!target_reconfiguring && rx_active == active && dma_received_count() == count && count > rx_published) {
        publish_received(received_block[active] + rx_published, count - rx_published);
        rx_published = count;
      }
      __set_PRIMASK(irq_state);
    }
    osDelay(1U);
  }
}
static queued_uart_tx_t target_tx_queue;
static uint8_t target_tx_scratch[2U * QUEUED_UART_BLOCK];
volatile uint32_t target_uart_tx_errors, target_uart_tx_queue_peak;
volatile uint32_t target_uart_rx_errors;
volatile uint32_t target_uart_base_clock;
volatile int32_t target_uart_last_status;
volatile uint32_t target_uart_received_bytes;
volatile uint32_t target_uart_sent_bytes;
static void uart_event(uint32_t event)
{
  if ((event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_RX_FRAMING_ERROR
                | ARM_USART_EVENT_RX_PARITY_ERROR | ARM_USART_EVENT_RX_BREAK)) != 0U) {
    target_uart_rx_errors++;
  }
  if ((event & ARM_USART_EVENT_SEND_COMPLETE) != 0U) {
    queued_uart_complete(&target_tx_queue);
  }

}

bool target_uart_init(target_uart_rx_callback_t on_receive)
{
  if (on_receive == NULL) {
    return false;
  }
  receive_callback = on_receive;
  target_uart_last_status = Driver_USART0.Initialize(uart_event);
  if (target_uart_last_status != ARM_DRIVER_OK) {
    return false;
  }
  target_uart_last_status = Driver_USART0.PowerControl(ARM_POWER_FULL);
  if (target_uart_last_status != ARM_DRIVER_OK) {
    return false;
  }
  target_uart_base_clock = RSI_CLK_GetBaseClock(M4_USART0);
  target_uart_last_status = Driver_USART0.Control(
    ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE
    | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, WIRELESS_UART_DRIVER_BAUD);
  if (target_uart_last_status != ARM_DRIVER_OK) {
    return false;
  }
  target_uart_last_status = Driver_USART0.Control(ARM_USART_CONTROL_TX, 1U);
  if (target_uart_last_status != ARM_DRIVER_OK) {
    return false;
  }
  target_uart_last_status = Driver_USART0.Control(ARM_USART_CONTROL_RX, 1U);
  if (target_uart_last_status != ARM_DRIVER_OK) {
    return false;
  }
  /* STM32 TX is high impedance during reset and before ROM UART setup.
   * Hold our RX idle HIGH rather than interpreting that interval as bytes. */
  RSI_EGPIO_PadDriverDisableState(29U, Pullup);
  target_uart_last_status = target_rx_start();
  if (target_uart_last_status != ARM_DRIVER_OK) return false;
  if (!queued_uart_init(&target_tx_queue, &Driver_USART0, target_tx_scratch, UART0,
        &target_uart_sent_bytes, &target_uart_tx_hash, &target_uart_tx_errors,
        &target_uart_tx_queue_peak)) return false;
  const osThreadAttr_t idle_attributes = {
    .name = "target_rx_idle", .stack_size = 1024, .priority = osPriorityAboveNormal
  };
  return osThreadNew(target_rx_idle_task, NULL, &idle_attributes) != NULL;
}

bool target_uart_set_bootloader_mode(bool enabled)
{
  if (target_tx_queue.head != target_tx_queue.tail || target_tx_queue.failed
      || Driver_USART0.GetStatus().tx_busy) return false;
  target_uart_prepare_bootloader(enabled);
  uint32_t irq_state = __get_PRIMASK();
  __disable_irq();
  rx_pingpong_running = false;
  target_uart_last_status = Driver_USART0.Control(ARM_USART_ABORT_RECEIVE, 0U);
  if (target_uart_last_status == ARM_DRIVER_OK) {
    rx_active = rx_published = 0;
    target_uart_last_status = Driver_USART0.Control(
      ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8
      | (enabled ? ARM_USART_PARITY_EVEN : ARM_USART_PARITY_NONE)
      | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, enabled ? 115200U : WIRELESS_UART_DRIVER_BAUD);
    if (target_uart_last_status == ARM_DRIVER_OK) {
      RSI_EGPIO_PadDriverDisableState(29U, Pullup);
      target_uart_last_status = target_rx_start();
    }
  }
  /* Changing LCR can latch DesignWare busy-detect (IIR low nibble 7).
   * SDK 4.0.2 UartIrqHandler does not clear it; reading USR does. Clear
   * before unmasking IRQs, otherwise the UART IRQ can starve every task. */
  (void)UART0->USR;
  __set_PRIMASK(irq_state);
  if(target_uart_last_status == ARM_DRIVER_OK) {
    target_baud_valid=true;
    target_even_parity=enabled;target_uart_prepare_bootloader(enabled);
    target_nominal_baud=enabled ? 115200U : WIRELESS_UART_NOMINAL_BAUD;
    target_driver_baud=enabled ? 115200U : WIRELESS_UART_DRIVER_BAUD;
  }
  return target_uart_last_status == ARM_DRIVER_OK;
}

bool target_uart_write(const uint8_t *data, uint32_t length)
{
  return queued_uart_write(&target_tx_queue, data, length);
}

/* Idle-only change. Never reset the target, silently discard RX, or abort TX. */
uint8_t target_uart_set_baud(uint32_t baud,uint32_t *applied,uint32_t *driver)
{
  *applied=target_baud_valid ? target_nominal_baud | (target_even_parity ? PC_LINE_EVEN : 0U) : 0U;
  *driver=target_baud_valid ? target_driver_baud : 0U;
  if(!baud) return !target_baud_valid ? BAUD_UART_ERROR : BAUD_OK;
  if(baud & 0x40000000U) return BAUD_UNSUPPORTED;
  bool even=(baud & PC_LINE_EVEN)!=0;
  uint32_t requested_line=baud;baud &= PC_LINE_BAUD_MASK;
  if(!pc_baud_supported(baud)) return BAUD_UNSUPPORTED;
  /* SDK4.0.2 USART_GetStatus testsLSR bit7 for TEMT; hardware TEMT is bit6.
   * Use the explicit bit6 check below; bit7 is a receive-error summary. */
  uint32_t started=osKernelGetTickCount();
  while(target_tx_queue.head!=target_tx_queue.tail || target_tx_queue.active_count
        || !(UART0->LSR & (1U<<6)) || (UART0->LSR & 1U) || (UART0->USR & 1U) || !target_uart_rx_idle()) {
    if(osKernelGetTickCount()-started >= osKernelGetTickFreq()/2U) return BAUD_BUSY;
    osDelay(1U);
  }
  if(target_tx_queue.failed) return BAUD_UART_ERROR;
  uint32_t state=__get_PRIMASK();__disable_irq();
  if(dma_received_count()!=rx_published || !(UART0->LSR & (1U<<6))
      || (UART0->LSR & 1U) || (UART0->USR & 1U)) {
    __set_PRIMASK(state);return BAUD_BUSY;
  }
  target_reconfiguring=true;rx_pingpong_running=false;target_baud_valid=false;
  uint32_t old=target_driver_baud;
  /* Preserve8N1 throughput headroom. ROM8E1 uses the exact requested baud. */
  uint32_t requested=even ? baud : (uint32_t)(((uint64_t)baud*10075U+5000U)/10000U);
  int32_t result=Driver_USART0.Control(ARM_USART_ABORT_RECEIVE,0U);
  if(result==ARM_DRIVER_OK) result=Driver_USART0.Control(
      ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8
      | (even ? ARM_USART_PARITY_EVEN : ARM_USART_PARITY_NONE) | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE,requested);
  if(result!=ARM_DRIVER_OK) {
    (void)Driver_USART0.Control(ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8
      | (target_even_parity ? ARM_USART_PARITY_EVEN : ARM_USART_PARITY_NONE)
      | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE,old);
  }
  int32_t rx_result=target_rx_start();
  (void)UART0->USR;
  target_reconfiguring=false;__set_PRIMASK(state);
  target_uart_last_status=result!=ARM_DRIVER_OK ? result : rx_result;
  if(target_uart_last_status!=ARM_DRIVER_OK) { *applied=*driver=0U;return BAUD_UART_ERROR; }
  target_even_parity=even;target_uart_prepare_bootloader(target_bootloader_selected && even);
  target_baud_valid=true;target_nominal_baud=baud;target_driver_baud=requested;
  *applied=requested_line;*driver=requested;return BAUD_OK;
}
