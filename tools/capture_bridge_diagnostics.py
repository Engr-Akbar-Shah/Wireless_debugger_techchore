#!/usr/bin/env python3
"""Read current/retained radio counters after a test, never during throughput."""
import argparse
import datetime
import json
import hashlib
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ['magic', 'sequence', 'role', 'reason', 'tick', 'protocol_errors',
          'heartbeat_timeouts', 'network_overflows', 'send_errors', 'errno',
          'network_payload_bytes', 'network_payload_hash', 'uart_rx_bytes',
          'uart_rx_hash', 'uart_tx_bytes', 'uart_tx_hash', 'uart_errors',
          'uart_ring_overflows', 'uart_tx_errors', 'tcp_tx_bytes', 'tcp_tx_hash', 'stop_reason', 'stop_detail', 'tx_reserved', 'peer_delivered', 'delivered', 'data_tx_sequence', 'data_rx_sequence', 'network_pending_bytes', 'send_calls', 'last_rx_tick', 'duplicate_data_frames', 'ap_connect_events', 'ap_disconnect_events', 'ap_connected_mac_lo', 'ap_connected_mac_hi', 'ap_disconnected_mac_lo', 'ap_disconnected_mac_hi', 'ap_event_id', 'ap_event_tick', 'c_alloc_failures', 'c_alloc_last_size', 'station_event_source', 'station_event_id', 'station_event_status', 'station_data_length', 'station_data0', 'station_data1', 'station_data2', 'station_data3', 'station_event_tick', 'c_alloc_failure_tick']
NAMES = ['pc_radio_batches','pc_radio_full_batches','pc_radio_idle_batches','bridge_bus_pending_since','bridge_bus_wait_max_ticks','bridge_bus_wait_flags','bridge_bus_wait_global','bridge_bus_tx_packets','bridge_bus_last_tx_tick','bridge_bus_rx_packets','bridge_bus_last_rx_tick','global_queue_block','interrupt_status','tx_socket_data_queues_status','bridge_c_alloc_failures', 'bridge_c_alloc_last_size', 'bridge_c_alloc_failure_tick', 'bridge_station_event_source', 'bridge_station_event_id', 'bridge_station_event_status', 'bridge_station_data_length', 'bridge_station_data0', 'bridge_station_data1', 'bridge_station_data2', 'bridge_station_data3', 'bridge_station_event_tick', 'heap_end.0', 'bridge_ap_connect_events', 'bridge_ap_disconnect_events', 'bridge_ap_connected_mac_lo', 'bridge_ap_connected_mac_hi', 'bridge_ap_disconnected_mac_lo', 'bridge_ap_disconnected_mac_hi', 'bridge_ap_event_id', 'bridge_ap_event_tick', 'bridge_ap_keepalive_before', 'bridge_ap_idle_before', 'bridge_ap_keepalive_applied', 'bridge_duplicate_data_frames', 'bridge_stale_credit_frames', 'bridge_stop_reason', 'bridge_stop_detail', 'bridge_data_sequence_errors', 'network_data_tx_sequence', 'network_data_rx_sequence', 'queued_uart_write_timeouts', 'bridge_network_stage', 'bridge_listener_stage', 'network_last_rx_tick', 'network_last_tx_tick', 'bridge_power_status', 'bridge_power_profile', 'bridge_firmware_status', 'bridge_send_calls', 'bridge_send_max_ticks', 'bridge_send_slow_calls',
         'bridge_wifi_rssi', 'bridge_wifi_rssi_status',
         'network_tx_total', 'network_peer_delivered', 'network_delivered_total', 'bridge_credit_waits',
         'bridge_credit_wait_max_ticks', 'bridge_credit_wait_total_ticks', 'bridge_credit_wait_events',
         'bridge_vcom_base_clock', 'target_uart_base_clock',
         'xFreeBytesRemaining', 'xMinimumEverFreeBytesRemaining',
         'uart_bench_edge_count', 'uart_bench_cycles', 'uart_bench_lcr_ext', 'uart_bench_clock', 'uart_bench_request', 'uart_bench_mode', 'uart_bench_active', 'uart_bench_status', 'uart_bench_bytes', 'uart_bench_start_tick', 'uart_bench_end_tick', 'uart_bench_rx_bytes', 'uart_bench_rx_hash',
         'queued_uart_setup_total_lo', 'queued_uart_setup_total_hi', 'queued_uart_full_blocks', 'queued_uart_full_cycles_lo', 'queued_uart_full_cycles_hi', 'queued_uart_dma_max_cycles', 'queued_uart_chain_idle', 'queued_uart_chain_fifo_min',
         'queued_uart_chained_blocks', 'queued_uart_initial_blocks',
         'queued_uart_unprepared_completions', 'queued_uart_setup_max_timer_ticks', 'queued_uart_timer_hz',
         'g_tcp_socket', 'bridge_protocol_errors', 'bridge_heartbeat_timeouts',
         'bridge_network_rx_overflows', 'bridge_network_send_errors',
         'bridge_network_last_errno', 'bridge_payload_received_bytes',
         'bridge_payload_rx_hash', 'bridge_tcp_sent_bytes', 'bridge_tcp_tx_hash',
         'bridge_vcom_received_bytes', 'bridge_vcom_rx_hash', 'bridge_vcom_sent_bytes',
         'bridge_vcom_tx_hash', 'bridge_vcom_ring_overflows', 'bridge_vcom_rx_dma_blocks', 'bridge_vcom_rx_dma_stalls', 'bridge_vcom_rx_errors',
         'bridge_vcom_tx_errors', 'bridge_vcom_tx_queue_peak', 'bridge_uart_ring_overflows',
         'target_uart_rx_dma_blocks', 'target_uart_received_bytes', 'target_uart_rx_hash', 'target_uart_rx_dma_stalls', 'target_uart_sent_bytes',
         'target_uart_tx_hash', 'target_uart_tx_errors', 'target_uart_tx_queue_peak', 'target_uart_rx_errors', 'target_uart_last_status']

def decode_transport_trace(words):
    """Decode the optional188-byte3.11 trace; no device access."""
    if len(words) != 47 or any(word is None for word in words):
        return {'read_ok': False}
    events = ['tx_queued', 'rx_callback', 'rx_payload', 'delivered',
              'credit_sent', 'credit_received', 'return_admitted', 'return_sent',
              'uart_overflow', 'credit_stall', 'return_stall']
    head = words[2]
    records = [words[15+i*4:19+i*4] for i in range(8)]
    ordered = [records[i % 8] for i in range(max(0, head-8), head)]
    return {'read_ok': True, 'valid': words[0] == 0x54524331, 'frozen': bool(words[1]),
            'head': head, 'clock': 'Local radio kernel ticks; boards have independent time origins',
            'last_stage': {events[i]: {'tick': words[3+i], 'total': words[9+i]} for i in range(6)},
            'records': [{'tick': r[0], 'event': events[r[1]-1] if 1 <= r[1] <= len(events) else r[1],
                         'total': r[2], 'detail': r[3]} for r in ordered]}


def capture(project, serial, elf_path=None):
    elf = Path(elf_path) if elf_path else ROOT / project / 'cmake_gcc/build/base' / f'{project}.out'
    symbols = {}
    for line in subprocess.check_output(['arm-none-eabi-nm', '-n', str(elf)], text=True).splitlines():
        parts = line.split()
        if len(parts) == 3:
            symbols[parts[2]] = int(parts[0], 16)
    selected = {name: symbols[name] for name in NAMES if name in symbols}
    if 'uart_bench_edges' in symbols:
        selected.update({f'uart_bench_edge_{i}': symbols['uart_bench_edges'] + i*4 for i in range(64)})
    record = symbols['bridge_failure_record']
    script = 'connect\n' + ''.join(f'mem32 0x{address:X}, 1\n' for address in selected.values())
    trace_address = symbols.get('bridge_transport_trace')
    if trace_address is not None:
        script += f'mem32 0x{trace_address:X}, 47\n'
    return_failure_address = symbols.get('pc_return_failure')
    if return_failure_address is not None:
        script += f'mem32 0x{return_failure_address:X}, 12\n'
    script += ''.join(f'mem32 0x{record+i*4:X}, 1\n' for i in range(len(FIELDS))) + 'exit\n'
    with tempfile.TemporaryDirectory(prefix='bridge-evidence-') as folder:
        path = Path(folder) / 'read.jlink'; path.write_text(script)
        run = subprocess.run(['/opt/SEGGER/JLink/JLinkExe', '-device', 'Cortex-M4',
            '-if', 'SWD', '-speed', '1000', '-SelectEmuBySN', serial,
            '-CommanderScript', str(path)], capture_output=True, text=True, timeout=30)
    memory = {}
    for address, values in re.findall(r'(?im)^([0-9a-f]{8})[ \t]*=[ \t]*([0-9a-f]{8}(?:[ \t]+[0-9a-f]{8})*)[ \t]*$', run.stdout):
        for i, value in enumerate(values.split()):
            memory[int(address, 16) + i*4] = int(value, 16)
    retained = {name: memory.get(record + i*4) for i, name in enumerate(FIELDS)}
    current = {name: memory.get(address) for name, address in selected.items()}
    for name in ('network_data_tx_sequence', 'network_data_rx_sequence', 'pc_data_tx_sequence', 'pc_data_tx_ack'):
        if current.get(name) is not None: current[name] &= 0xffff
    for name in ('g_tcp_socket', 'bridge_wifi_rssi'):
        if current.get(name) is not None and current[name] >= 0x80000000:
            current[name] -= 0x100000000
    readable = run.returncode == 0 and all(value is not None for value in current.values())
    result = {'elf_path': str(elf), 'elf_sha256': hashlib.sha256(elf.read_bytes()).hexdigest(), 'current': current,
            'retained_valid': retained['magic'] == 0x42524631,
            'retained': retained, 'read_ok': readable,
            'scope': 'Read after test. Retained values are last failed session, current values are since latest radio reset.'}
    if trace_address is not None:
        result['transport_trace'] = decode_transport_trace([memory.get(trace_address+i*4) for i in range(47)])
        readable = readable and result['transport_trace']['read_ok']
        result['read_ok'] = readable
    if return_failure_address is not None:
        keys = ['magic', 'queue_peak', 'enqueue_wait_max_ticks', 'ack_wait_max_ticks',
                'ack_wait_total_ticks', 'input_ack_max_ticks', 'service_gap_max_ticks',
                'admitted_bytes', 'sent_bytes', 'tx_sequence', 'tx_ack', 'uart_enqueue_max_ticks']
        result['return_failure'] = {k: memory.get(return_failure_address+i*4) for i, k in enumerate(keys)}
        result['return_failure']['valid'] = result['return_failure']['magic'] == 0x50524631
    if current.get('heap_end.0') is not None:
        result['c_heap'] = {'capacity_bytes': symbols['__HeapLimit']-symbols['__HeapBase'], 'committed_bytes':current['heap_end.0']-symbols['__HeapBase'], 'sbrk_remaining_bytes':symbols['__HeapLimit']-current['heap_end.0']}
    if not readable:
        result['debug_error'] = (run.stdout + run.stderr)[-1800:]
    return result

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, required=True)
    args = parser.parse_args()
    result = {'timestamp_utc': datetime.datetime.now(datetime.timezone.utc).isoformat()}
    for board, project, serial in [('A', 'Debugger-SiW917_AccessPoint', '440332408'),
                                    ('B', 'Debugger-SiW917_station', '440332624')]:
        result[board] = capture(project, serial)
    args.report.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
