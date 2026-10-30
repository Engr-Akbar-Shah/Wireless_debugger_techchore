/* Retained failure evidence. This NOBITS section is outside startup BSS.
 * Valid only across MCU resets, not power removal. Inspect magic before use. */
#ifndef BRIDGE_DIAGNOSTICS_H
#define BRIDGE_DIAGNOSTICS_H

volatile uint32_t bridge_full_reset_request;
volatile uint32_t bridge_failure_record[52] __attribute__((section(".noinit"), used));
static void bridge_save_failure(uint32_t role, uint32_t reason,
    uint32_t uart_rx_bytes, uint32_t uart_rx_hash,
    uint32_t uart_tx_bytes, uint32_t uart_tx_hash,
    uint32_t uart_errors, uint32_t ring_overflows, uint32_t tx_errors)
{
  __disable_irq();
  uint32_t sequence = bridge_failure_record[0] == 0x42524631U
      ? bridge_failure_record[1] + 1U : 1U;
  bridge_failure_record[0] = 0U;
  bridge_failure_record[1] = sequence;
  bridge_failure_record[2] = role;
  bridge_failure_record[3] = reason;
  bridge_failure_record[4] = osKernelGetTickCount();
  bridge_failure_record[5] = bridge_protocol_errors;
  bridge_failure_record[6] = bridge_heartbeat_timeouts;
  bridge_failure_record[7] = bridge_network_rx_overflows;
  bridge_failure_record[8] = bridge_network_send_errors;
  bridge_failure_record[9] = (uint32_t)bridge_network_last_errno;
  bridge_failure_record[10] = bridge_payload_received_bytes;
  bridge_failure_record[11] = bridge_payload_rx_hash;
  bridge_failure_record[12] = uart_rx_bytes;
  bridge_failure_record[13] = uart_rx_hash;
  bridge_failure_record[14] = uart_tx_bytes;
  bridge_failure_record[15] = uart_tx_hash;
  bridge_failure_record[16] = uart_errors;
  bridge_failure_record[17] = ring_overflows;
  bridge_failure_record[18] = tx_errors;
  bridge_failure_record[19] = bridge_tcp_sent_bytes;
  bridge_failure_record[20] = bridge_tcp_tx_hash;
  bridge_failure_record[21] = bridge_stop_reason;
  bridge_failure_record[22] = bridge_stop_detail;
  bridge_failure_record[23] = network_tx_total;
  bridge_failure_record[24] = network_peer_delivered;
  bridge_failure_record[25] = network_delivered_total;
  bridge_failure_record[26] = network_data_tx_sequence;
  bridge_failure_record[27] = network_data_rx_sequence;
  bridge_failure_record[28] = (network_rx_head + NETWORK_RX_RING_SIZE - network_rx_tail) % NETWORK_RX_RING_SIZE;
  bridge_failure_record[29] = bridge_send_calls;
  bridge_failure_record[30] = network_last_rx_tick;
  bridge_failure_record[31] = bridge_duplicate_data_frames;
  bridge_failure_record[32] = bridge_ap_connect_events;
  bridge_failure_record[33] = bridge_ap_disconnect_events;
  bridge_failure_record[34] = bridge_ap_connected_mac_lo;
  bridge_failure_record[35] = bridge_ap_connected_mac_hi;
  bridge_failure_record[36] = bridge_ap_disconnected_mac_lo;
  bridge_failure_record[37] = bridge_ap_disconnected_mac_hi;
  bridge_failure_record[38] = bridge_ap_event_id;
  bridge_failure_record[39] = bridge_ap_event_tick;
  bridge_failure_record[40] = bridge_c_alloc_failures;
  bridge_failure_record[41] = bridge_c_alloc_last_size;
  bridge_failure_record[42] = bridge_station_event_source;
  bridge_failure_record[43] = bridge_station_event_id;
  bridge_failure_record[44] = bridge_station_event_status;
  bridge_failure_record[45] = bridge_station_data_length;
  bridge_failure_record[46] = bridge_station_data0;
  bridge_failure_record[47] = bridge_station_data1;
  bridge_failure_record[48] = bridge_station_data2;
  bridge_failure_record[49] = bridge_station_data3;
  bridge_failure_record[50] = bridge_station_event_tick;
  bridge_failure_record[51] = bridge_c_alloc_failure_tick;
  __DMB();
  bridge_failure_record[0] = 0x42524631U;
  __DSB();
  NVIC_SystemReset();
}
#endif
