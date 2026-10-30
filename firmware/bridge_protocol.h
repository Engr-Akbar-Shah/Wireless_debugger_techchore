/* Internal radio protocol. USB and target UART remain arbitrary byte streams. */
#ifndef BRIDGE_PROTOCOL_H
#define BRIDGE_PROTOCOL_H
#include <stdint.h>
#include <string.h>
#define BRIDGE_FRAME_HEADER 8U
#define BRIDGE_FRAME_MAX 1452U /* IPv4 TCP MSS 1460 minus our 8-byte header */
#define BRIDGE_CREDIT_FRAME_SIZE (BRIDGE_FRAME_HEADER + 4U)
#define BRIDGE_DATA_CHUNK (BRIDGE_FRAME_MAX - BRIDGE_CREDIT_FRAME_SIZE)
#define BRIDGE_PROTOCOL_VERSION 6U
#define BRIDGE_PAYLOAD_WINDOW 28672U
enum { BRIDGE_DATA = 1, BRIDGE_HEARTBEAT = 2, BRIDGE_CONTROL = 3, BRIDGE_ACK = 4, BRIDGE_CREDIT = 5, BRIDGE_BAUD = 6, BRIDGE_BAUD_ACK = 7, BRIDGE_HELLO = 8 };
enum { BRIDGE_RESET_APP = 1, BRIDGE_BOOT_ROM = 2, BRIDGE_RELEASE = 3 };
static inline int bridge_credit_can_send(uint32_t sent, uint32_t delivered, uint32_t length)
{
  return length > 0U && length <= BRIDGE_FRAME_MAX
      && (uint32_t)(sent - delivered) <= BRIDGE_PAYLOAD_WINDOW - length;
}
static inline int bridge_credit_valid(uint32_t sent, uint32_t previous, uint32_t delivered)
{
  return (uint32_t)(delivered - previous) <= (uint32_t)(sent - previous);
}
/* A repeated last DATA frame is safe only if every byte is identical.
 * Caller keeps the previous accepted payload until the next accepted DATA. */
static inline int bridge_data_sequence_check(uint16_t previous, uint16_t received,
    const uint8_t *last, uint16_t last_length, const uint8_t *payload, uint16_t length)
{
  if (received == (uint16_t)(previous + 1U)) return 1;
  if (last_length && received == previous && length == last_length
      && memcmp(last, payload, length) == 0) return 0;
  return -1;
}
static inline int bridge_credit_stale(uint32_t previous, uint32_t delivered)
{
  uint32_t age = previous - delivered;
  return age > 0U && age <= BRIDGE_PAYLOAD_WINDOW;
}
typedef struct {
  uint8_t header[BRIDGE_FRAME_HEADER], payload[BRIDGE_FRAME_MAX];
  uint16_t used, length, sequence;
  uint8_t type;
} bridge_parser_t;
static inline int bridge_frame_shape(uint8_t type, uint16_t length)
{
  return (type == BRIDGE_DATA && length > 0 && length <= BRIDGE_FRAME_MAX)
      || (type == BRIDGE_HEARTBEAT && length == 0)
      || (type == BRIDGE_CONTROL && length == 1)
      || (type == BRIDGE_ACK && length == 2)
      || (type == BRIDGE_CREDIT && length == 4)
      || (type == BRIDGE_HELLO && length == 4)
      || (type == BRIDGE_BAUD && length == 4)
      || (type == BRIDGE_BAUD_ACK && length == 9);
}
static inline uint16_t bridge_encode(uint8_t *out, uint8_t type,
                                    const uint8_t *data, uint16_t length,
                                    uint16_t sequence)
{
  if (!bridge_frame_shape(type, length) || (length && !data)) return 0;
  out[0] = 'W'; out[1] = 'U'; out[2] = BRIDGE_PROTOCOL_VERSION; out[3] = type;
  out[4] = length & 255U; out[5] = length >> 8;
  out[6] = sequence & 255U; out[7] = sequence >> 8;
  if (length) memcpy(out + BRIDGE_FRAME_HEADER, data, length);
  return BRIDGE_FRAME_HEADER + length;
}
/* Prefix the current credit to DATA in the same socket write. A full UART
 * batch plus both existing protocol headers is exactly one1460-byte TCP MSS.
 * Duplicate cumulative credits are valid. No credit bytes reach the UART. */
static inline uint16_t bridge_encode_data_with_credit(uint8_t *out,
    const uint8_t *data, uint16_t length, uint32_t delivered)
{
  if (!data || !length || length > BRIDGE_DATA_CHUNK) return 0;
  uint8_t credit[4] = {delivered, delivered >> 8, delivered >> 16, delivered >> 24};
  uint16_t prefix = bridge_encode(out, BRIDGE_CREDIT, credit, sizeof(credit), 0U);
  return prefix + bridge_encode(out + prefix, BRIDGE_DATA, data, length, 0U);
}
/* -1 malformed (close connection), 0 incomplete, 1 complete. A completed
 * payload remains valid until the next feed; caller consumes it immediately. */
static inline int bridge_feed(bridge_parser_t *parser, uint8_t byte)
{
  if (parser->used < BRIDGE_FRAME_HEADER) {
    parser->header[parser->used++] = byte;
    if (parser->used != BRIDGE_FRAME_HEADER) return 0;
    if (parser->header[0] != 'W' || parser->header[1] != 'U'
        || parser->header[2] != BRIDGE_PROTOCOL_VERSION) return -1;
    parser->type = parser->header[3];
    parser->length = parser->header[4] | ((uint16_t)parser->header[5] << 8);
    parser->sequence = parser->header[6] | ((uint16_t)parser->header[7] << 8);
    if (!bridge_frame_shape(parser->type, parser->length)) return -1;
    if (!parser->length) { parser->used = 0; return 1; }
    return 0;
  }
  parser->payload[parser->used++ - BRIDGE_FRAME_HEADER] = byte;
  if (parser->used == BRIDGE_FRAME_HEADER + parser->length) {
    parser->used = 0;
    return 1;
  }
  return 0;
}
#endif
