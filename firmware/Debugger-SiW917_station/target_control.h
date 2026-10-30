#ifndef TARGET_CONTROL_H
#define TARGET_CONTROL_H
#include "rsi_egpio.h"
#include "rsi_rom_egpio.h"
#include "rsi_pll.h"
#include "rsi_rom_clks.h"
#include "target_uart.h"

/* UG581 Table 3.2: GPIO25 EXP3, GPIO26 EXP5, no shared board feature.
 * GPIO25 -> MB1136 CN6.3 NRST; GPIO26 -> CN7.7 BOOT0.
 * NRST is emulated open drain: output LOW or input/released, never HIGH. */
volatile uint32_t target_control_executed;
volatile uint32_t target_bootloader_selected;
static void target_control_release(void)
{
  RSI_EGPIO_SetPin(EGPIO, 1, 10, 0);
  target_bootloader_selected=0;target_uart_prepare_bootloader(false);
  RSI_EGPIO_SetDir(EGPIO, 1, 9, EGPIO_CONFIG_DIR_INPUT);
}
static void target_control_init(void)
{
  (void)RSI_CLK_PeripheralClkEnable3(M4CLK, EGPIO_CLK_ENABLE);
  RSI_EGPIO_SetDir(EGPIO, 1, 9, EGPIO_CONFIG_DIR_INPUT);
  RSI_EGPIO_SetDir(EGPIO, 1, 10, EGPIO_CONFIG_DIR_INPUT);
  RSI_EGPIO_SetPin(EGPIO, 1, 9, 0);
  RSI_EGPIO_SetPin(EGPIO, 1, 10, 0);
  RSI_EGPIO_SetPinMux(EGPIO, 1, 9, EGPIO_PIN_MUX_MODE0);
  RSI_EGPIO_SetPinMux(EGPIO, 1, 10, EGPIO_PIN_MUX_MODE0);
  RSI_EGPIO_PadSelectionEnable(25);
  RSI_EGPIO_PadSelectionEnable(26);
  /* GPIO25..30 have a second routing gate for the host interface pads.
   * Pad ownership and GPIO mode alone do not connect EGPIO to these pins. */
  RSI_EGPIO_HostPadsGpioModeEnable(25);
  RSI_EGPIO_HostPadsGpioModeEnable(26);
  RSI_EGPIO_PadReceiverEnable(25);
  RSI_EGPIO_PadReceiverEnable(26);
  RSI_EGPIO_SetDir(EGPIO, 1, 10, EGPIO_CONFIG_DIR_OUTPUT);
  target_control_release();
}
static uint8_t target_control_execute(uint8_t operation)
{
  if(operation>=PC_MODEM_OPERATION && operation<=PC_MODEM_OPERATION+3U) {
    uint8_t bits=operation-PC_MODEM_OPERATION;
    target_bootloader_selected=(bits>>1)&1U;
    target_uart_prepare_bootloader(target_bootloader_selected!=0U);
    /* Host assertion1: DTR pulls reset LOW; RTS drives BOOT0 HIGH. */
    RSI_EGPIO_SetPin(EGPIO,1,10,(bits>>1)&1U);
    if(!(bits&1U)) osDelay((osKernelGetTickFreq()+999U)/1000U);
    RSI_EGPIO_SetPin(EGPIO,1,9,0);
    RSI_EGPIO_SetDir(EGPIO,1,9,(bits&1U) ? EGPIO_CONFIG_DIR_OUTPUT : EGPIO_CONFIG_DIR_INPUT);
    target_control_executed++;return 0;
  }
  if (operation == BRIDGE_RELEASE) { target_control_release(); return 0; }
  if (operation != BRIDGE_RESET_APP && operation != BRIDGE_BOOT_ROM) return 1;
  target_uart_prepare_bootloader(operation == BRIDGE_BOOT_ROM);
  RSI_EGPIO_SetPin(EGPIO, 1, 9, 0);
  RSI_EGPIO_SetDir(EGPIO, 1, 9, EGPIO_CONFIG_DIR_OUTPUT);
  RSI_EGPIO_SetPin(EGPIO, 1, 10, operation == BRIDGE_BOOT_ROM);
  osDelay((osKernelGetTickFreq() * 20U + 999U) / 1000U);
  bool configured = target_uart_set_bootloader_mode(operation == BRIDGE_BOOT_ROM);
  RSI_EGPIO_SetDir(EGPIO, 1, 9, EGPIO_CONFIG_DIR_INPUT);
  osDelay((osKernelGetTickFreq() * 20U + 999U) / 1000U);
  RSI_EGPIO_SetPin(EGPIO, 1, 10, 0); /* boot selection has been sampled */
  if (!configured) return 2;
  target_bootloader_selected = operation == BRIDGE_BOOT_ROM;
  target_control_executed++;
  return 0;
}
#endif
