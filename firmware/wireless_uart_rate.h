#ifndef WIRELESS_UART_RATE_H
#define WIRELESS_UART_RATE_H
/* Pico and A use a separate fixed2M8N1 framed UART. At full nominal
 *9216008N1 payload,256-byte DATA+ACK framing alone needs100800B/s per
 *direction before escaping, exceeding the old1M UART's100000B/s limit.
 *2M gives200000B/s. A uses160MHz INTFPLL /16 /5 for exact calculated2M.
 *Target8N1 retains its recorded0.75% driver headroom; ROM8E1 is exact115200.
 *The selected laptop USB baud is forwarded to B; it is not this link rate. */
#define WIRELESS_UART_INTERNAL_USB_BAUD 2000000U
#define WIRELESS_UART_NOMINAL_BAUD 921600U
#define WIRELESS_UART_DRIVER_BAUD 928512U
#endif
