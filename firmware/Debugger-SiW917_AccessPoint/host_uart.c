#include "host_uart.h"

#include "USART.h"
#include "clock_update.h"
#include "cmsis_os2.h"
#include "rsi_udma.h"
#include "rsi_egpio.h"
#include "rsi_rom_egpio.h"
#include "../queued_uart_tx.h"
#include "../wireless_uart_rate.h"
#include "../pc_link.h"
static volatile bool target_reconfiguring;





extern ARM_DRIVER_USART Driver_USART0;

static host_uart_rx_callback_t receive_callback;
extern volatile uint32_t bridge_vcom_received_bytes;
static volatile bool boot_sync_pending;
static volatile uint32_t boot_sync_discarded;
volatile uint32_t host_uart_boot_prefix_discarded;
void host_uart_prepare_bootloader(bool enabled)
{
  boot_sync_pending = enabled;
  boot_sync_discarded = 0U;
}
volatile uint32_t bridge_vcom_rx_hash = 2166136261U;
volatile uint32_t bridge_vcom_tx_hash = 2166136261U;
#define TARGET_RX_BLOCK_SIZE 256U
static uint8_t received_block[2][TARGET_RX_BLOCK_SIZE];
static volatile uint32_t rx_active, rx_published;
static volatile uint32_t rx_activity_tick;
extern RSI_UDMA_DESC_T UDMA0_Table[];
extern UDMA_Channel_Info udma0_chnl_info[];
static void target_rx_dma_event(uint32_t event, uint32_t channel);
static volatile bool rx_pingpong_running;
static RSI_UDMA_CHA_CONFIG_DATA_T rx_dma_template;
volatile uint32_t host_uart_rx_dma_stalls, host_uart_rx_dma_blocks;
extern volatile uint32_t bridge_vcom_rx_errors;
static volatile RSI_UDMA_DESC_T *rx_descriptor(uint32_t slot)
{
  return &UDMA0_Table[24U + slot * 32U];
}
static void publish_received(const uint8_t *data, uint32_t length)
{
  bridge_vcom_received_bytes += length;
  for (uint32_t i = 0; i < length; ++i)
    bridge_vcom_rx_hash = (bridge_vcom_rx_hash ^ data[i]) * 16777619U;
  uint32_t first = 0U;
  while (boot_sync_pending && first < length) {
    if (data[first] != 0x79U && data[first] != 0x1FU && boot_sync_discarded < 512U) {
      boot_sync_discarded++; host_uart_boot_prefix_discarded++; first++;
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
static void host_uart_dma_rx_irq(uint32_t pending)
{
  if (!rx_pingpong_running || !pending) return;
  if ((UART0->LSR & 0x1EU) != 0U) bridge_vcom_rx_errors++;
  for (uint32_t i = 0; i < 2U; ++i) {
    uint32_t slot = rx_active;
    if (rx_descriptor(slot)->vsUDMAChaConfigData1.transferType != 0U) break;
    if (rx_descriptor(slot ^ 1U)->vsUDMAChaConfigData1.transferType == 0U)
      host_uart_rx_dma_stalls++;
    uint32_t published = rx_published;
    publish_received(received_block[slot] + published, TARGET_RX_BLOCK_SIZE - published);
    rx_arm_descriptor(slot);
    rx_active = slot ^ 1U; rx_published = 0U;
    rx_activity_tick = osKernelGetTickCount();
    host_uart_rx_dma_blocks++;
  }
  __DMB();
  UDMA0->CHNL_ENABLE_SET = 1UL << 24;
}
static void target_rx_dma_event(uint32_t event, uint32_t channel)
{
  (void)channel;
  if (event == UDMA_EVENT_XFER_DONE) host_uart_dma_rx_irq(1U);
  else if (event == UDMA_EVENT_ERROR) bridge_vcom_rx_errors++;
}
static uint32_t dma_received_count(void)
{
  RSI_UDMA_CHA_CONFIG_DATA_T control = rx_descriptor(rx_active)->vsUDMAChaConfigData1;
  if (control.transferType == 0U) return TARGET_RX_BLOCK_SIZE;
  uint32_t remaining = control.totalNumOfDMATrans + 1U;
  return remaining <= TARGET_RX_BLOCK_SIZE ? TARGET_RX_BLOCK_SIZE - remaining : 0U;
}
bool host_uart_rx_idle(void)
{
  uint32_t idle_ticks = (osKernelGetTickFreq() * 4U + 999U) / 1000U;
  return (uint32_t)(osKernelGetTickCount() - rx_activity_tick) >= idle_ticks
      && dma_received_count() <= rx_published;
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
volatile uint32_t bridge_vcom_tx_errors, bridge_vcom_tx_queue_peak;
volatile uint32_t bridge_vcom_rx_errors;
volatile uint32_t bridge_vcom_base_clock;
volatile int32_t bridge_vcom_init_status;
volatile uint32_t bridge_vcom_received_bytes;
volatile uint32_t bridge_vcom_sent_bytes;
static void uart_event(uint32_t event)
{
  if ((event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_RX_FRAMING_ERROR
                | ARM_USART_EVENT_RX_PARITY_ERROR | ARM_USART_EVENT_RX_BREAK)) != 0U) {
    bridge_vcom_rx_errors++;
  }
  if ((event & ARM_USART_EVENT_SEND_COMPLETE) != 0U) {
    queued_uart_complete(&target_tx_queue);
  }

}

bool host_uart_init(host_uart_rx_callback_t on_receive)
{
  if (on_receive == NULL) {
    return false;
  }
  receive_callback = on_receive;
  bridge_vcom_init_status = Driver_USART0.Initialize(uart_event);
  if (bridge_vcom_init_status != ARM_DRIVER_OK) {
    return false;
  }
  bridge_vcom_init_status = Driver_USART0.PowerControl(ARM_POWER_FULL);
  if (bridge_vcom_init_status != ARM_DRIVER_OK) {
    return false;
  }
  bridge_vcom_base_clock = RSI_CLK_GetBaseClock(M4_USART0);
  bridge_vcom_init_status = Driver_USART0.Control(
    ARM_USART_MODE_ASYNCHRONOUS | ARM_USART_DATA_BITS_8 | ARM_USART_PARITY_NONE
    | ARM_USART_STOP_BITS_1 | ARM_USART_FLOW_CONTROL_NONE, WIRELESS_UART_INTERNAL_USB_BAUD);
  if (bridge_vcom_init_status != ARM_DRIVER_OK) {
    return false;
  }
  bridge_vcom_init_status = Driver_USART0.Control(ARM_USART_CONTROL_TX, 1U);
  if (bridge_vcom_init_status != ARM_DRIVER_OK) {
    return false;
  }
  bridge_vcom_init_status = Driver_USART0.Control(ARM_USART_CONTROL_RX, 1U);
  if (bridge_vcom_init_status != ARM_DRIVER_OK) {
    return false;
  }
  /* STM32 TX is high impedance during reset and before ROM UART setup.
   * Hold our RX idle HIGH rather than interpreting that interval as bytes. */
  RSI_EGPIO_PadDriverDisableState(29U, Pullup);
  bridge_vcom_init_status = target_rx_start();
  if (bridge_vcom_init_status != ARM_DRIVER_OK) return false;
  if (!queued_uart_init(&target_tx_queue, &Driver_USART0, target_tx_scratch, UART0,
        &bridge_vcom_sent_bytes, &bridge_vcom_tx_hash, &bridge_vcom_tx_errors,
        &bridge_vcom_tx_queue_peak)) return false;
  const osThreadAttr_t idle_attributes = {
    .name = "target_rx_idle", .stack_size = 1024, .priority = osPriorityAboveNormal
  };
  return osThreadNew(target_rx_idle_task, NULL, &idle_attributes) != NULL;
}

bool host_uart_write(const uint8_t *data,uint32_t length)
{ return queued_uart_write(&target_tx_queue,data,length); }
