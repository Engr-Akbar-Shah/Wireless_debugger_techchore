#ifndef TARGET_UART_H
#define TARGET_UART_H

#include <stdbool.h>
#include <stdint.h>

/* BRD2605A: USART0 TX=GPIO30 (EXP13), RX=GPIO29 (EXP11).
 * Initial9216008N1; idle-time baud selection supported in this variant.
 * ROM bootloader preset remains1152008E1. */
typedef void (*target_uart_rx_callback_t)(const uint8_t *data, uint32_t length);

bool target_uart_init(target_uart_rx_callback_t on_receive);
uint8_t target_uart_set_baud(uint32_t baud,uint32_t *applied,uint32_t *driver);
bool target_uart_write(const uint8_t *data, uint32_t length);
bool target_uart_rx_idle(void);
/* Call only with target held in reset and no UART transmit in progress. */
bool target_uart_set_bootloader_mode(bool enabled);
void target_uart_prepare_bootloader(bool enabled);

/* Inspect in a debugger if an initial hardware test fails. */
extern volatile uint32_t target_bootloader_selected;
extern volatile uint32_t target_uart_rx_errors;
extern volatile uint32_t target_uart_tx_errors;
extern volatile int32_t target_uart_last_status;

#endif
