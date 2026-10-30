#ifndef BRIDGE_ENGINE_H
#define BRIDGE_ENGINE_H
#include <stdint.h>
#include <stdbool.h>
#include "pc_link.h"
#define BRIDGE_USB_RING 8192U
#define BRIDGE_USB_IDLE_MS 2U
#define BRIDGE_MODEM_SETTLE_MS 10U
typedef struct {
  uint32_t desired_line,applied_line,driver;
  bool boot_sync_pending;uint32_t boot_prefix_discarded;
  uint8_t desired_modem,applied_modem; /* bit0 hold reset, bit1 BOOT0 high */
  uint8_t usb_lines,usb_lines_used;     /* raw USB bit0 DTR, bit1 RTS */
  uint32_t usb_lines_at;
  uint8_t auto_boot; /* 0 off, 1 reset then BOOT0, 2 in ROM, 3 reset to application */
  bool mounted,synced,valid_line,fault;
  uint8_t waiting;uint16_t control_id;
  uint32_t sent_at,retry_at,last_ping,last_pong,now,request_line;
  uint8_t request_modem;
  uint16_t tx_sequence,tx_ack,rx_sequence;
  uint32_t usb_rx_bytes,usb_tx_bytes,errors,session_resets;
  uint32_t last_usb_rx,tx_batches,tx_full_batches,tx_idle_batches,tx_config_flush_batches;
  uint8_t pending[PC_MAX_DATA];unsigned pending_length;
  uint8_t output[BRIDGE_USB_RING];uint32_t output_head,output_tail;
} bridge_engine_t;
extern bridge_engine_t bridge;
void bridge_init(void);
void bridge_line_changed(uint32_t baud,uint8_t data,uint8_t parity,uint8_t stop);
void bridge_modem_changed(bool dtr,bool rts);
void bridge_usb_lines(bool dtr,bool rts);
void bridge_mounted(bool mounted);
bool bridge_packet(uint8_t type,uint16_t id,const uint8_t *payload,unsigned length);
void bridge_step(uint32_t now);
bool bridge_ready(void);
void bridge_status(void);
void bridge_transport_fault(const char *reason);
/* Platform hooks: bounded/nonblocking. No debug text on the data interface. */
bool bridge_port_send(uint8_t,uint16_t,const uint8_t *,unsigned);
unsigned bridge_usb_read(uint8_t *,unsigned);
unsigned bridge_usb_write(const uint8_t *,unsigned);
void bridge_usb_flush(void);
void bridge_usb_discard_output(void);
void bridge_log(const char *,...);
#endif
