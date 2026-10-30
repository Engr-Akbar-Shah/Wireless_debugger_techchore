# WiSeConnect 4.0.2 uses an auto-cleared event bit for socket RX notification,
# but consumes only one queued packet per socket. Coalesced events can leave
# buffers stranded. Drain each queue before waiting for another notification.
# Generate the adaptation; keep both installed SDK copies unchanged.
set(bridge_events_vendor "${CMAKE_CURRENT_SOURCE_DIR}/../wiseconnect3_sdk_4.0.2/components/sli_si91x_wifi_event_handler/src/sli_si91x_wifi_event_handler.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${bridge_events_vendor}")
file(READ "${bridge_events_vendor}" bridge_events_source)
set(bridge_events_old "if (sli_si91x_remove_from_queue(&sli_si91x_sockets[i]->rx_data_queue, &buffer) == SL_STATUS_OK)")
string(FIND "${bridge_events_source}" "${bridge_events_old}" bridge_events_position)
if(bridge_events_position EQUAL -1)
  message(FATAL_ERROR "Vendor socket RX notification handler changed; review queue-drain fix")
endif()
string(REPLACE "${bridge_events_old}" "while (sli_si91x_remove_from_queue(&sli_si91x_sockets[i]->rx_data_queue, &buffer) == SL_STATUS_OK)" bridge_events_source "${bridge_events_source}")
# Bounded instrumentation only: identify M4->NWP queue delay versus radio delay.
set(bridge_events_probe_old "        // Check if the bus is ready for a packet")
set(bridge_events_probe_new "        uint32_t bridge_now=osKernelGetTickCount();
        if(!bridge_bus_pending_since)bridge_bus_pending_since=bridge_now;
        uint32_t bridge_wait=bridge_now-bridge_bus_pending_since;
        if(bridge_wait>bridge_bus_wait_max_ticks) {bridge_bus_wait_max_ticks=bridge_wait;bridge_bus_wait_flags=interrupt_status;bridge_bus_wait_global=global_queue_block;}
        // Check if the bus is ready for a packet")
string(REPLACE "${bridge_events_probe_old}" "${bridge_events_probe_new}" bridge_events_source "${bridge_events_source}")
set(bridge_events_sent_old "        sl_status_t status = bus_write_data_frame(&sli_si91x_sockets[i]->tx_data_queue);")
set(bridge_events_sent_new "${bridge_events_sent_old}
        if(status==SL_STATUS_OK){bridge_bus_tx_packets++;bridge_bus_last_tx_tick=osKernelGetTickCount();bridge_bus_pending_since=0;}")
string(REPLACE "${bridge_events_sent_old}" "${bridge_events_sent_new}" bridge_events_source "${bridge_events_source}")
set(bridge_events_rx_old "    const sl_wifi_system_packet_t *response = (const sl_wifi_system_packet_t *)data;")
string(REPLACE "${bridge_events_rx_old}" "    bridge_bus_rx_packets++;bridge_bus_last_rx_tick=osKernelGetTickCount();
${bridge_events_rx_old}" bridge_events_source "${bridge_events_source}")
string(PREPEND bridge_events_source "#include <stdint.h>
volatile uint32_t bridge_bus_pending_since,bridge_bus_wait_max_ticks,bridge_bus_wait_flags,bridge_bus_wait_global,bridge_bus_tx_packets,bridge_bus_last_tx_tick,bridge_bus_rx_packets,bridge_bus_last_rx_tick;
")
get_target_property(bridge_events_sources slc SOURCES)
list(FILTER bridge_events_sources EXCLUDE REGEX "/sli_si91x_wifi_event_handler[.]c$")
set_property(TARGET slc PROPERTY SOURCES "${bridge_events_sources}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/bridge_wifi_events.c" "${bridge_events_source}")
target_sources(slc PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/bridge_wifi_events.c")
