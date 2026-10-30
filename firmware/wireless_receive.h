/* Async TCP reception: copy promptly, enqueue UART data from the application task.
 * Silicon Labs AN1429 recommends asynchronous sockets for concurrent traffic.
 * Single callback producer / single application consumer; never block SDK callbacks.
 */
#ifndef WIRELESS_RECEIVE_H
#define WIRELESS_RECEIVE_H
#include <string.h>
#include "sl_wifi.h"
#include "bridge_protocol.h"
#include "pc_link.h"
#include "credit_datagram.h"
#include "bridge_allocator.h"
#include "errno.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_utility.h"
#include "transport_trace.h"
volatile uint32_t bridge_power_status = SL_STATUS_NOT_INITIALIZED;
volatile uint32_t bridge_power_profile = 0xffffffffU;
volatile uint32_t bridge_firmware_status = SL_STATUS_NOT_INITIALIZED;
sl_wifi_firmware_version_t bridge_nwp_firmware;
static bool network_initialize_policy(bool apply_to_nwp)
{
  sl_wifi_performance_profile_v2_t profile = {0};
  bridge_firmware_status = sl_wifi_get_firmware_version(&bridge_nwp_firmware);
  bridge_power_status = sl_wifi_get_performance_profile_v2(&profile);
  if (bridge_power_status != SL_STATUS_OK) return false;
  /* This getter reads SDK memory, not the NWP. A cached zero alone does
   * not establish that the high-performance command was applied. */
  if (apply_to_nwp || profile.profile != SL_WIFI_SYSTEM_HIGH_PERFORMANCE) {
    profile.profile = SL_WIFI_SYSTEM_HIGH_PERFORMANCE;
    bridge_power_status = sl_wifi_set_performance_profile_v2(&profile);
    if (bridge_power_status != SL_STATUS_OK) return false;
    bridge_power_status = sl_wifi_get_performance_profile_v2(&profile);
  }
  bridge_power_profile = profile.profile;
  return bridge_power_status == SL_STATUS_OK
      && profile.profile == SL_WIFI_SYSTEM_HIGH_PERFORMANCE;
}
static sl_status_t network_socket_configure(void)
{
  /* Original passing multithread_sockets allocation and ordered TCP credits. */
  sl_si91x_socket_config_t config = {
    .total_sockets = 3, .total_tcp_sockets = 3,
    .tcp_tx_only_sockets = 1, .tcp_rx_only_sockets = 1,
    .tcp_rx_high_performance_sockets = 1,
    .tcp_rx_window_size_cap = 44, .tcp_rx_window_div_factor = 44
  };
  return sl_si91x_config_socket(config);
}
static int network_socket_high_performance(int socket_id)
{
  uint32_t enabled = 1U << 7;
  /* A 1s max-RTO cap was tried (30 Oct): throughput fell to ~7.5KB/s. Keep the default. */
  return sl_si91x_setsockopt(socket_id, SOL_SOCKET,
      SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET, &enabled, sizeof(enabled));
}
#define NETWORK_RX_RING_SIZE 32768U
static uint8_t network_rx_ring[NETWORK_RX_RING_SIZE];
static volatile uint32_t network_rx_head, network_rx_tail;
static volatile bool network_peer_closed;
/* First cause survives secondary failures during teardown. */
volatile uint32_t bridge_stop_reason, bridge_stop_detail;
volatile uint32_t bridge_ap_connect_events, bridge_ap_disconnect_events;
volatile uint32_t bridge_ap_connected_mac_lo, bridge_ap_connected_mac_hi;
volatile uint32_t bridge_ap_disconnected_mac_lo, bridge_ap_disconnected_mac_hi;
volatile uint32_t bridge_ap_event_id, bridge_ap_event_tick;
volatile uint32_t bridge_ap_keepalive_before, bridge_ap_idle_before;
volatile uint32_t bridge_ap_keepalive_applied;
volatile uint32_t bridge_station_event_source, bridge_station_event_id, bridge_station_event_status;
volatile uint32_t bridge_station_data_length, bridge_station_data0, bridge_station_data1;
volatile uint32_t bridge_station_data2, bridge_station_data3, bridge_station_event_tick;
static void __attribute__((unused)) network_record_station_event(uint32_t source, uint32_t event,
    uint32_t status, const void *data, uint32_t length)
{
  uint32_t words[4] = {0};
  if (data) {
    const uint8_t *bytes = data;
    for (uint32_t i=0; i<length && i<16U; ++i) words[i/4U] |= (uint32_t)bytes[i] << (8U*(i%4U));
  }
  bridge_station_event_source=source; bridge_station_event_id=event; bridge_station_event_status=status;
  bridge_station_data_length=length; bridge_station_data0=words[0]; bridge_station_data1=words[1];
  bridge_station_data2=words[2]; bridge_station_data3=words[3]; bridge_station_event_tick=osKernelGetTickCount();
}
volatile uint32_t bridge_data_sequence_errors;
volatile uint32_t bridge_duplicate_data_frames, bridge_stale_credit_frames;
static uint16_t network_last_payload_length;
static uint16_t network_data_tx_sequence, network_data_rx_sequence;
static void network_stop(uint32_t reason, uint32_t detail)
{
  uint32_t state = __get_PRIMASK(); __disable_irq();
  if (!bridge_stop_reason) { bridge_stop_reason = reason; bridge_stop_detail = detail; }
  network_peer_closed = true;
  __set_PRIMASK(state);
}
volatile uint32_t bridge_network_rx_overflows;
volatile uint32_t bridge_network_received_bytes;
volatile uint32_t bridge_network_rx_hash = 2166136261U;
volatile uint32_t bridge_network_send_errors;
volatile int32_t bridge_network_last_errno;
volatile uint32_t bridge_payload_received_bytes;
volatile uint32_t bridge_payload_rx_hash = 2166136261U;
volatile uint32_t bridge_send_calls, bridge_send_max_ticks, bridge_send_slow_calls;
static void network_receive_callback(uint32_t socket_id, uint8_t *data,
                                     uint32_t length,
                                     const sl_si91x_socket_metadata_t *metadata)
{
  (void)socket_id;
  (void)metadata;
  if (!length) { network_stop(1U, socket_id); return; }
  for (uint32_t i = 0; i < length; ++i)
    bridge_network_rx_hash = (bridge_network_rx_hash ^ data[i]) * 16777619U;
  uint32_t head = network_rx_head;
  uint32_t free_bytes = (network_rx_tail + NETWORK_RX_RING_SIZE - head - 1U)
                       % NETWORK_RX_RING_SIZE;
  if (length > free_bytes) {
    bridge_network_rx_overflows += length;
    network_stop(2U, length);
    return;
  }
  uint32_t first = NETWORK_RX_RING_SIZE - head;
  if (first > length) first = length;
  memcpy(network_rx_ring + head, data, first);
  memcpy(network_rx_ring, data + first, length - first);
  __DMB();
  network_rx_head = (head + length) % NETWORK_RX_RING_SIZE;
  bridge_network_received_bytes += length;
  bridge_trace_event(TRACE_RX_CALLBACK,bridge_network_received_bytes,length,false);
}
static void network_remote_closed(int socket_id, uint16_t port, uint32_t sent)
{
  (void)port; (void)sent;
  network_stop(3U, (uint32_t)socket_id);
}

/* Only the UART-to-network task sends frames. The receive owner queues replies;
 * no other task can interleave a header with a different frame's payload. */
static bridge_parser_t network_parser;
static volatile uint32_t network_last_rx_tick, network_last_tx_tick;
static uint8_t (*network_control_handler)(uint8_t operation);
static void (*network_disconnect_handler)(void);
static volatile bool network_ack_pending;
static uint16_t network_ack_sequence, network_control_sequence, network_expected_ack;
static uint16_t network_last_control_sequence;
static bool network_seen_control;
static uint8_t network_ack_operation, network_ack_status, network_last_control_status;
static uint32_t network_control_sent_tick;
volatile uint32_t bridge_control_request; /* A bench mailbox: 1 app reset, 2 ROM, 3 release */
volatile uint32_t bridge_control_status;  /* 0 idle, 1 awaiting ACK, 2 ACK OK, 3 failed */
volatile uint32_t bridge_control_ack_sequence;
volatile uint32_t bridge_protocol_errors, bridge_heartbeat_timeouts;
/* Cumulative delivered-byte credits wrap modulo2^32. The28KiB window
 * covers the measured260ms credit-return pause at92160 UART bytes/sec.
 * It remains below the32KiB network ring including protocol overhead.
 * Credits advance on downstream queue admission (A8KiB payload, B16KiB UART),
 * independently of physical transmission and Pico acknowledgement. */
static volatile uint32_t network_tx_total, network_peer_delivered, network_delivered_total;
static uint32_t network_delivered_advertised;
volatile uint32_t bridge_credit_waits;
volatile uint32_t bridge_credit_wait_max_ticks, bridge_credit_wait_total_ticks;
volatile uint32_t bridge_credit_wait_events;
static bool network_elapsed(uint32_t since, uint32_t milliseconds)
{
  return (uint32_t)(osKernelGetTickCount() - since)
      >= (osKernelGetTickFreq() * milliseconds + 999U) / 1000U;
}
static int network_send_frame(int socket_id, uint8_t type, const uint8_t *data,
                              uint16_t length, uint16_t sequence)
{
  static uint8_t frame[BRIDGE_FRAME_MAX + BRIDGE_FRAME_HEADER];
  uint32_t delivered = network_delivered_total;
  uint16_t size = type == BRIDGE_DATA
      ? bridge_encode_data_with_credit(frame, data, length, delivered)
      : bridge_encode(frame, type, data, length, sequence);
  if (!size) return -1;
  if (type == BRIDGE_DATA) {
    if (!bridge_credit_can_send(network_tx_total, network_peer_delivered, length))
      return -1;
    /* Reserve before publishing: an RX task can process a fast peer credit
     * immediately after send() queues the frame. Failed sends end the session. */
    network_tx_total += length;
    uint16_t data_sequence = ++network_data_tx_sequence;
    frame[BRIDGE_CREDIT_FRAME_SIZE + 6U] = (uint8_t)data_sequence;
    frame[BRIDGE_CREDIT_FRAME_SIZE + 7U] = (uint8_t)(data_sequence >> 8);
  }
  uint32_t sent = 0, progress = osKernelGetTickCount();
  while (sent < size && !network_peer_closed) {
    uint32_t begin = osKernelGetTickCount();
    int count = send(socket_id, frame + sent, size - sent, 0);
    uint32_t ticks = osKernelGetTickCount() - begin;
    bridge_send_calls++;
    if (ticks > bridge_send_max_ticks) bridge_send_max_ticks = ticks;
    if (ticks >= (osKernelGetTickFreq() + 49U) / 50U) bridge_send_slow_calls++;
    if (count < 0 && errno == ENOBUFS && !network_elapsed(progress, 200U)) {
      bridge_network_send_errors++;
      bridge_network_last_errno = errno;
      osDelay(2); continue;
    }
    if (count <= 0) {
      bridge_network_last_errno = errno;
      network_stop(4U, (uint32_t)errno);
      return -1;
    }
    sent += (uint32_t)count;
    progress = osKernelGetTickCount();
  }
  if (sent != size) return -1;
  if(type==BRIDGE_DATA)network_delivered_advertised=delivered;
  network_last_tx_tick = osKernelGetTickCount();
  if(type==BRIDGE_DATA)bridge_trace_event(TRACE_TX_QUEUED,network_tx_total,length,false);
  return (int)length;
}
/* Credits must not sit behind the DATA whose delivery they acknowledge.
 * UDP counters are cumulative, session-bound and repeated every20ms. A lost,
 * duplicate, reordered or old-session datagram cannot revoke/overgrant credit. */
static int network_credit_socket=-1;
static struct sockaddr_in network_credit_peer;
static uint32_t network_credit_nonce,network_credit_peer_nonce,network_credit_last_tick;
static bool network_credit_hello_sent;
__attribute__((section(".noinit"))) static uint32_t network_credit_serial;
volatile uint32_t bridge_credit_datagrams_sent,bridge_credit_datagrams_received;
volatile uint32_t bridge_credit_datagrams_ignored,bridge_credit_datagram_send_failures;
static void __attribute__((unused)) network_credit_receive(uint32_t socket_id,uint8_t *data,uint32_t length,
    const sl_si91x_socket_metadata_t *metadata)
{
 (void)socket_id;(void)metadata;uint32_t value;
 if(credit_datagram_decode(data,length,network_credit_nonce,network_tx_total,network_peer_delivered,&value)) {
  network_peer_delivered=value;bridge_credit_datagrams_received++;
  bridge_trace_event(TRACE_CREDIT_RECEIVED,value,network_tx_total,false);
 } else bridge_credit_datagrams_ignored++;
}
static bool network_credit_open(int tcp_socket,bool ap_role)
{ (void)tcp_socket;(void)ap_role;return true; /* Baseline ordered TCP credit path. */ }
static bool __attribute__((unused)) network_credit_service(int socket_id)
{
 if(!network_credit_hello_sent) {
  uint8_t hello[4];credit_put32(hello,network_credit_nonce);
  if(network_send_frame(socket_id,BRIDGE_HELLO,hello,sizeof(hello),0)<0)return false;
  network_credit_hello_sent=true;
 }
 if(network_credit_socket<0 || !network_credit_peer_nonce)return true;
 if(network_delivered_total!=network_delivered_advertised || network_elapsed(network_credit_last_tick,20U)) {
  uint8_t packet[BRIDGE_CREDIT_DATAGRAM_SIZE];
  uint32_t delivered=network_delivered_total;
  credit_datagram_encode(packet,network_credit_peer_nonce,delivered);
  network_credit_last_tick=osKernelGetTickCount();
  int count=sendto(network_credit_socket,packet,sizeof(packet),0,
      (struct sockaddr *)&network_credit_peer,sizeof(network_credit_peer));
  if(count==(int)sizeof(packet)) {
    network_delivered_advertised=delivered;bridge_credit_datagrams_sent++;
    bridge_trace_event(TRACE_CREDIT_SENT,delivered,network_tx_total,false);
  }
  else bridge_credit_datagram_send_failures++;
 }
 return true;
}
/* Baud configuration travels independently of DATA and its credits. */
static uint8_t (*network_baud_handler)(uint32_t, uint32_t *, uint32_t *);
static volatile uint32_t network_baud_request, network_baud_status;
static volatile uint32_t network_baud_applied, network_baud_driver;
static volatile uint8_t network_baud_result;
static uint16_t network_baud_sequence, network_baud_expected, network_baud_ack_id;
static uint16_t network_baud_last_id;
static uint32_t network_baud_sent_tick, network_baud_last_value;
static bool network_baud_seen;
static volatile bool network_baud_ack_pending;
static uint8_t network_baud_response[9];
static bool network_service(int socket_id, bool data_pending)
{
  if(bridge_trace_clear_request)bridge_trace_init();
  if (network_peer_closed) return false;
  uint32_t delivered=network_delivered_total;
  if(!data_pending && delivered!=network_delivered_advertised) {
    uint8_t credit[4];pc_put32(credit,delivered);
    if(network_send_frame(socket_id,BRIDGE_CREDIT,credit,sizeof(credit),0U)<0)return false;
    network_delivered_advertised=delivered;
  }
  if (network_ack_pending) {
    uint8_t response[2] = {network_ack_operation, network_ack_status};
    uint16_t sequence = network_ack_sequence;
    if (network_send_frame(socket_id, BRIDGE_ACK, response, 2U, sequence) < 0) return false;
    network_ack_pending = false;
  }
  if (network_baud_ack_pending) {
    if (network_send_frame(socket_id, BRIDGE_BAUD_ACK, network_baud_response, 9U, network_baud_ack_id)<0) return false;
    network_baud_ack_pending=false;
  }
  if (network_baud_status==1U && !network_baud_expected) {
    uint8_t request[4]; pc_put32(request,network_baud_request);
    if (++network_baud_sequence==0) ++network_baud_sequence;
    network_baud_expected=network_baud_sequence;
    network_baud_sent_tick=osKernelGetTickCount();
    if(network_send_frame(socket_id,BRIDGE_BAUD,request,4U,network_baud_expected)<0) return false;
  }
  if(network_baud_expected && network_elapsed(network_baud_sent_tick,3000U)) {
    network_baud_result=BAUD_TIMEOUT;network_baud_status=3U;network_baud_expected=0;
    network_baud_applied=network_baud_driver=0; /* Applied state is unknown. */
  }
  uint32_t operation = bridge_control_request;
  if (operation && !network_expected_ack) {
    bridge_control_request = 0;
    if ((operation > BRIDGE_RELEASE && (operation < PC_MODEM_OPERATION || operation > PC_MODEM_OPERATION+3U))) { bridge_control_status = 3U; return true; }
    uint8_t command = (uint8_t)operation;
    if (++network_control_sequence == 0) ++network_control_sequence;
    network_expected_ack = network_control_sequence;
    bridge_control_status = 1U;
    network_control_sent_tick = osKernelGetTickCount();
    if (network_send_frame(socket_id, BRIDGE_CONTROL, &command, 1U,
                           network_control_sequence) < 0) return false;
  }
  if (network_expected_ack && network_elapsed(network_control_sent_tick, 5000U)) {
    network_expected_ack = 0;
    bridge_control_status = 3U;
  }
  return !network_elapsed(network_last_tx_tick, 1000U)
      || network_send_frame(socket_id, BRIDGE_HEARTBEAT, NULL, 0U, 0U) == 0;
}
static bool __attribute__((unused)) network_wait_payload_credit(int socket_id, uint32_t length)
{
  if (length > BRIDGE_PAYLOAD_WINDOW) return false;
  uint32_t started = osKernelGetTickCount();
  bool waited = false, ok = true;
  while (!network_peer_closed
      && !bridge_credit_can_send(network_tx_total, network_peer_delivered, length)) {
    if (!waited) { waited = true; bridge_credit_wait_events++; }
    bridge_credit_waits++;
    if(network_elapsed(started,250U))
      bridge_trace_event(TRACE_CREDIT_STALL,network_tx_total,network_peer_delivered,true);
    /* If DATA cannot be sent, standalone credits must still flow. */
    if (!network_service(socket_id, false)) { ok = false; break; }
    osDelay(1U);
  }
  if (waited) {
    uint32_t ticks = osKernelGetTickCount() - started;
    bridge_credit_wait_total_ticks += ticks;
    if (ticks > bridge_credit_wait_max_ticks) bridge_credit_wait_max_ticks = ticks;
  }
  return ok && !network_peer_closed;
}
static void network_payload_delivered(uint32_t length)
{
  network_delivered_total += length;
  bridge_trace_event(TRACE_DELIVERED,network_delivered_total,length,false);
}
static void network_receive_reset(void)
{
  bridge_trace_init();
  if (network_disconnect_handler) network_disconnect_handler();
  network_rx_tail = network_rx_head;
  network_peer_closed = false;
  memset(&network_parser, 0, sizeof(network_parser));
  network_last_rx_tick = network_last_tx_tick = osKernelGetTickCount();
  network_baud_ack_pending=false;network_baud_seen=false;network_baud_expected=0;
  network_baud_result=BAUD_OFFLINE;network_baud_applied=network_baud_driver=0;
  if(network_baud_status==1U) network_baud_status=3U;
  network_ack_pending = false;
  network_seen_control = false;
  network_expected_ack = 0;
  network_tx_total = network_peer_delivered = network_delivered_total = 0U;
  network_delivered_advertised = 0U;
  network_credit_socket=-1;network_credit_peer_nonce=0;network_credit_hello_sent=false;
  network_credit_nonce=(++network_credit_serial)^DWT->CYCCNT^osKernelGetTickCount();
  if(!network_credit_nonce)network_credit_nonce=1U;
  network_credit_last_tick=osKernelGetTickCount();
  bridge_stop_reason = bridge_stop_detail = 0U;
  network_data_tx_sequence = network_data_rx_sequence = 0U;
  network_last_payload_length = 0U;
  if (bridge_control_request || bridge_control_status == 1U) bridge_control_status = 3U;
  bridge_control_request = 0; /* Never replay a reset request into a new session. */
  sl_si91x_set_remote_termination_callback(network_remote_closed);
}
static int network_receive(uint8_t *data, uint32_t capacity)
{
  if(network_peer_closed)return -1;
  while (network_rx_tail != network_rx_head) {
    uint32_t tail = network_rx_tail;
    uint8_t byte = network_rx_ring[tail];
    __DMB();
    network_rx_tail = (tail + 1U) % NETWORK_RX_RING_SIZE;
    int frame = bridge_feed(&network_parser, byte);
    if (frame < 0) { bridge_protocol_errors++; network_stop(5U, network_parser.used); return -1; }
    if (!frame) continue;
    network_last_rx_tick = osKernelGetTickCount();
    uint16_t length = network_parser.length;
    if (network_parser.type == BRIDGE_DATA) {
      if (length > capacity) { bridge_protocol_errors++; network_stop(6U, length); return -1; }
      uint16_t expected = (uint16_t)(network_data_rx_sequence + 1U);
      int order = bridge_data_sequence_check(network_data_rx_sequence, network_parser.sequence,
          data, network_last_payload_length, network_parser.payload, length);
      if (order == 0) { bridge_duplicate_data_frames++; continue; }
      if (order < 0) {
        bridge_data_sequence_errors++;
        network_stop(network_parser.sequence == network_data_rx_sequence ? 13U : 7U,
            ((uint32_t)expected << 16) | network_parser.sequence);
        return -1;
      }
      network_data_rx_sequence = expected;
      network_last_payload_length = length;
      for (uint16_t i = 0; i < length; ++i)
        bridge_payload_rx_hash = (bridge_payload_rx_hash ^ network_parser.payload[i]) * 16777619U;
      bridge_payload_received_bytes += length;
      bridge_trace_event(TRACE_RX_PAYLOAD,bridge_payload_received_bytes,length,false);
      memcpy(data, network_parser.payload, length);
      return length;
    }
    if(network_parser.type==BRIDGE_HELLO) {
      network_credit_peer_nonce=credit_get32(network_parser.payload);
      continue;
    }
    if (network_parser.type == BRIDGE_BAUD) {
      uint32_t value=pc_get32(network_parser.payload);
      if(network_baud_ack_pending) {
        if(network_baud_last_id==network_parser.sequence && value==network_baud_last_value) continue;
        network_stop(14U,network_parser.sequence);return -1;
      }
      if(network_baud_seen && network_baud_last_id==network_parser.sequence && value!=network_baud_last_value) {
        network_stop(14U,network_parser.sequence);return -1;
      }
      if(!network_baud_seen || network_baud_last_id!=network_parser.sequence) {
        uint32_t applied=0,driver=0;
        network_baud_response[0]=network_baud_handler ? network_baud_handler(value,&applied,&driver) : BAUD_UNSUPPORTED;
        pc_put32(network_baud_response+1,applied);pc_put32(network_baud_response+5,driver);
        network_baud_last_id=network_parser.sequence;network_baud_last_value=value;network_baud_seen=true;
      }
      network_baud_ack_id=network_parser.sequence;__DMB();network_baud_ack_pending=true;
    } else if(network_parser.type==BRIDGE_BAUD_ACK && network_baud_expected==network_parser.sequence) {
      network_baud_result=network_parser.payload[0];
      network_baud_applied=pc_get32(network_parser.payload+1);
      network_baud_driver=pc_get32(network_parser.payload+5);
      network_baud_status=network_baud_result==BAUD_OK ? 2U : 3U;__DMB();network_baud_expected=0;
    } else if (network_parser.type == BRIDGE_CREDIT) {
      if(network_credit_socket>=0)continue; /* Out-of-band counters own credit. */
      const uint8_t *p = network_parser.payload;
      uint32_t delivered = p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
      if (!bridge_credit_valid(network_tx_total, network_peer_delivered, delivered)) {
        if (bridge_credit_stale(network_peer_delivered, delivered)) {
          bridge_stale_credit_frames++; continue;
        }
        bridge_protocol_errors++; network_stop(8U, delivered); return -1;
      }
      network_peer_delivered = delivered;
    } else if (network_parser.type == BRIDGE_CONTROL) {
      /* One outstanding control request per peer. Cached ACK prevents a retry
       * of the same command from producing another reset pulse. */
      if (network_ack_pending) { network_stop(9U, network_parser.sequence); return -1; }
      if (!network_seen_control || network_last_control_sequence != network_parser.sequence) {
        network_last_control_status = network_control_handler
            ? network_control_handler(network_parser.payload[0]) : 1U;
        network_last_control_sequence = network_parser.sequence;
        network_seen_control = true;
      }
      network_ack_sequence = network_parser.sequence;
      network_ack_operation = network_parser.payload[0];
      network_ack_status = network_last_control_status;
      __DMB();
      network_ack_pending = true;
    } else if (network_parser.type == BRIDGE_ACK
               && network_expected_ack == network_parser.sequence) {
      bridge_control_ack_sequence = network_parser.sequence;
      bridge_control_status = network_parser.payload[1] == 0U ? 2U : 3U;
      network_expected_ack = 0;
    }
  }
  /* A recovering TCP stall is delay, not loss: only 15s of silence ends the session. */
  if (!network_peer_closed && network_elapsed(network_last_rx_tick, 15000U)) {
    bridge_heartbeat_timeouts++;
    network_stop(10U, network_last_rx_tick);
    if (network_disconnect_handler) network_disconnect_handler();
  }
  return network_peer_closed ? -1 : 0;
}
#endif
