#ifndef HOST_UART_H
#define HOST_UART_H
#include <stdbool.h>
#include <stdint.h>
typedef void (*host_uart_rx_callback_t)(const uint8_t *,uint32_t);
bool host_uart_init(host_uart_rx_callback_t);
bool host_uart_write(const uint8_t *,uint32_t);
bool host_uart_rx_idle(void);
extern volatile uint32_t bridge_vcom_received_bytes,bridge_vcom_sent_bytes;
extern volatile uint32_t bridge_vcom_rx_hash,bridge_vcom_tx_hash;
extern volatile uint32_t bridge_vcom_rx_errors,bridge_vcom_tx_errors,bridge_vcom_base_clock;
extern volatile int32_t bridge_vcom_init_status;
#endif
