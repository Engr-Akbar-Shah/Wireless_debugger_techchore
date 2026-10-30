#!/usr/bin/env python3
"""Linux demo support for the Pico variant. No flashing; no work without a command."""
import argparse
import array
import datetime
import fcntl
import hashlib
import json
import math
import os
from pathlib import Path
import random
import statistics
import sys
import termios
import threading
import time

ROOT = Path(__file__).resolve().parents[1]
PICO = 'usb-Wireless_UART_prototype_Wireless_UART_Pico_bridge_E66488C15F39C034'
DATA = Path('/dev/serial/by-id') / (PICO + '-if00')
STATUS = Path('/dev/serial/by-id') / (PICO + '-if02')
FIXTURE = Path('/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_066CFF535767864967152953-if02')
RATES = [9600, 57600, 115200, 230400, 460800, 921600]


def release():
    # nRF must already own an open data handle. Do not set baud or read payload.
    fd = os.open(DATA, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    try:
        fcntl.ioctl(fd, termios.TIOCMBIC,
                    array.array('i', [termios.TIOCM_DTR | termios.TIOCM_RTS]))
    finally:
        os.close(fd)
    print('Requested DTR OFF and RTS OFF. Keep the nRF data port open.')


def set_lines(fd, dtr, rts):
    """Set DTR and RTS in one call: no intermediate state reaches the target."""
    state = array.array('i', [0])
    fcntl.ioctl(fd, termios.TIOCMGET, state)
    bits = state[0] & ~(termios.TIOCM_DTR | termios.TIOCM_RTS)
    bits |= (termios.TIOCM_DTR if dtr else 0) | (termios.TIOCM_RTS if rts else 0)
    fcntl.ioctl(fd, termios.TIOCMSET, array.array('i', [bits]))


def target_control(command):
    # Auto-reset mapping: DTR=RTS runs, RTS alone holds reset, DTR alone selects BOOT0.
    with serial_module().Serial(str(DATA), 115200, parity=serial_module().PARITY_EVEN
                                if command == 'boot' else serial_module().PARITY_NONE, timeout=.2) as port:
        time.sleep(.3)
        set_lines(port.fileno(), False, True)   # hold reset
        time.sleep(.3)
        if command == 'boot':
            set_lines(port.fileno(), True, False)   # release reset with BOOT0 high
            time.sleep(.5)
            print('STM32 is in its ROM bootloader (115200 8E1). Program with:')
            print(f'  STM32_Programmer_CLI -c port={DATA} br=115200 P=EVEN dtr=high rts=low -d <file> -v')
        else:
            set_lines(port.fileno(), False, False)  # release reset, BOOT0 low
            time.sleep(.5)
            print('STM32 reset into its application. Terminal use needs no further step.')


def serial_module():
    import serial
    return serial


def fixture(baud):
    with serial_module().Serial(str(FIXTURE), 115200, timeout=2, exclusive=True) as port:
        port.reset_input_buffer()
        port.write(f'BAUD {baud}\n'.encode())
        answer = port.readline().decode('ascii', 'replace').strip()
    fields = answer.split()
    if len(fields) != 4 or fields[:2] != ['OK', str(baud)]:
        raise RuntimeError('Fixture did not acknowledge baud: ' + answer)
    print('STM32 fixture: ' + answer)


def state(port, baud=None, timeout=40):
    deadline = time.monotonic() + timeout
    last = ''
    while time.monotonic() < deadline:
        port.write(b'STATUS\n')
        end = time.monotonic() + .4
        while time.monotonic() < end:
            line = port.readline().decode('ascii', 'replace').strip()
            if not line.startswith('STATUS '):
                continue
            last = line
            fields = dict(item.split('=', 1) for item in line.split()[1:] if '=' in item)
            if (fields.get('ready') == '1' and fields.get('dtr') == '0'
                    and fields.get('rts') == '0' and fields.get('parity') == 'N'
                    and (baud is None or fields.get('baud') == str(baud))):
                return fields
    raise TimeoutError('Bridge not ready with DTR/RTS released: ' + last)


def open_data(baud, write_timeout=3):
    serial = serial_module()
    p = serial.Serial(port=None, baudrate=baud, timeout=.02,
                      write_timeout=write_timeout, exclusive=True, rtscts=False, xonxoff=False)
    p.dtr = False
    p.rts = False
    p.port = str(DATA)
    try:
        p.open()
        with serial.Serial(str(STATUS), 115200, timeout=.05, exclusive=True) as console:
            applied = state(console, baud)
        time.sleep(.3)
        transition = p.read(p.in_waiting).hex()
        return p, applied, transition
    except BaseException:
        p.close()
        raise


def exact(port, count):
    result = bytearray()
    deadline = time.monotonic() + max(5, count * 20 / port.baudrate)
    while len(result) < count and time.monotonic() < deadline:
        result.extend(port.read(count - len(result)))
    return bytes(result)


class Stream:
    def __init__(self):
        self.rng = random.Random(917)
        self.pending = b''

    def read(self, count):
        out = bytearray()
        while count:
            if not self.pending:
                self.pending = self.rng.randbytes(65536)
            take = min(count, len(self.pending))
            out.extend(self.pending[:take])
            self.pending = self.pending[take:]
            count -= take
        return bytes(out)


def endurance(port, seconds, rate, allow_delay=False):
    total = int(seconds * rate)
    result = dict(requested_seconds=seconds, offered_Bps=rate,
                  expected_bytes_each_direction=total, sent_bytes=0, received_bytes=0)
    sent, received = hashlib.sha256(), hashlib.sha256()
    done, stop = threading.Event(), threading.Event()
    start = time.monotonic()

    def writer():
        source = Stream()
        try:
            while result['sent_bytes'] < total and not stop.is_set():
                offset = result['sent_bytes']
                if stop.wait(max(0, start + offset / rate - time.monotonic())):
                    break
                data = source.read(min(512, total - offset))
                n = port.write(data)
                result['sent_bytes'] += n
                sent.update(data[:n])
                if n != len(data):
                    raise RuntimeError('Short serial write')
            port.flush()
            result['writer_seconds'] = time.monotonic() - start
        except Exception as exc:
            result['writer_error'] = str(exc)
        finally:
            done.set()

    thread = threading.Thread(target=writer, daemon=True)
    thread.start()
    expected = Stream()
    last_rx = start
    next_log = start + 30
    try:
        while time.monotonic() - start < seconds + (120 if allow_delay else 10):
            data = port.read(max(1, min(port.in_waiting, 4096)))
            now = time.monotonic()
            if data:
                wanted = expected.read(len(data))
                offset = result['received_bytes']
                result['received_bytes'] += len(data)
                received.update(data)
                last_rx = now
                if data != wanted:
                    result['first_mismatch'] = offset + next(i for i, pair in enumerate(zip(data, wanted)) if pair[0] != pair[1])
                    break
            if now >= next_log:
                print(f"{now-start:.0f}s sent={result['sent_bytes']} received={result['received_bytes']}", flush=True)
                next_log = now + 30
            if now - last_rx > (15 if allow_delay else 5):
                result['error'] = 'No echo for five seconds'
                break
            if done.is_set() and result['received_bytes'] >= result['sent_bytes']:
                break
    finally:
        stop.set()
        thread.join(timeout=4)
    time.sleep(.15)
    extra = port.read(port.in_waiting)
    result.update(elapsed_seconds=time.monotonic()-start, sent_sha256=sent.hexdigest(),
                  received_sha256=received.hexdigest(), extra_bytes=len(extra))
    result['exact_bytes_passed'] = (not thread.is_alive() and not result.get('writer_error')
        and not result.get('error') and 'first_mismatch' not in result and not extra
        and result['sent_bytes'] == result['received_bytes'] == total and sent.digest() == received.digest())
    # Reject a run whose source was slowed materially by backpressure.
    result['offered_load_met'] = (result['sent_bytes'] == total and not result.get('writer_error')
        and result.get('writer_seconds', math.inf) <= seconds + max(.25, seconds * .001))
    result['rate_requirement'] = 'delay allowed' if allow_delay else 'full nominal capacity'
    result['passed'] = result['exact_bytes_passed'] and (allow_delay or result['offered_load_met'])
    return result


def latency(port):
    rng, rows = random.Random(917), []
    for size in [1, 32, 256, 1024]:
        samples = []
        for _ in range(50):
            payload = rng.randbytes(size)
            start = time.perf_counter()
            if port.write(payload) != size or exact(port, size) != payload:
                raise RuntimeError(f'Latency echo mismatch for {size} bytes')
            samples.append((time.perf_counter()-start)*1000)
        ordered = sorted(samples)
        row = dict(payload_bytes=size, samples=50, median_ms=statistics.median(samples),
                   p95_ms=ordered[math.ceil(.95*len(ordered))-1], max_observed_ms=max(samples))
        rows.append(row)
        print(json.dumps(row), flush=True)
    return dict(passed=True, latency=rows, scope='Round trip including USB and echo; maximum observed, not a guaranteed bound')


def policy(report_path):
    # Read only after traffic stops; use symbols for the actual Pico variant.
    sys.path.insert(0, str(ROOT/'tools'))
    import capture_bridge_diagnostics as diag
    diag.ROOT = ROOT/'experiments/pico-bridge/firmware'
    diag.NAMES = ['bridge_power_status', 'bridge_power_profile']
    result = dict(scope='Software policy/status readback; no power waveform or latency qualification',
                  image_manifest=json.loads((ROOT/'experiments/pico-bridge/build-manifest.json').read_text()))
    for board, project, serial in [('A', 'Debugger-SiW917_AccessPoint', '440332408'),
                                    ('B', 'Debugger-SiW917_station', '440332624')]:
        elf = ROOT/f'experiments/pico-bridge/images/board-{board}-pico.out'
        manifest = result['image_manifest']
        if hashlib.sha256(elf.read_bytes()).hexdigest() != manifest['images'][elf.name]:
            raise RuntimeError('ELF identity mismatch: '+str(elf))
        row = diag.capture(project, serial, elf_path=elf)
        result[board] = row
        print(board, json.dumps(row['current']), 'read_ok=', row['read_ok'])
    result['passed'] = all(result[b]['read_ok'] and result[b]['current'].get('bridge_power_status') == 0
                          and result[b]['current'].get('bridge_power_profile') == 0 for b in ['A', 'B'])
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(json.dumps(result, indent=2)+'\n')
    print(('PASS' if result['passed'] else 'FAIL') + ' policy readback: ' + str(report_path))
    if not result['passed']:
        raise SystemExit(1)


def capture_counters():
    """Only called before/after timed traffic, using matching image symbols."""
    sys.path.insert(0, str(ROOT/'tools'))
    import capture_bridge_diagnostics as radio
    import capture_stm32_diagnostics as stm
    radio.ROOT = ROOT/'experiments/pico-bridge/firmware'
    for name in ['pc_return_head', 'pc_return_tail', 'pc_return_queue_peak', 'pc_return_admitted_bytes',
                 'pc_return_sent_bytes', 'pc_return_discarded_bytes', 'pc_return_enqueue_wait_max_ticks',
                 'pc_return_enqueue_timeouts', 'pc_data_tx_sequence', 'pc_data_tx_ack', 'pc_ack_wait_events']:
        if name not in radio.NAMES:
            radio.NAMES.append(name)
    for name in ['pc_bad_frames', 'host_uart_rx_dma_stalls', 'host_uart_rx_dma_blocks', 'bridge_forward_socket', 'bridge_reverse_socket', 'pc_mutex_max_ticks', 'pc_uart_enqueue_max_ticks', 'pc_ack_wait_max_ticks', 'pc_ack_wait_total_ticks', 'pc_input_ack_max_ticks', 'pc_service_gap_max_ticks', 'bridge_credit_datagrams_sent', 'bridge_credit_datagrams_received', 'bridge_credit_datagrams_ignored', 'bridge_credit_datagram_send_failures', 'network_delivered_advertised', 'network_credit_nonce', 'network_credit_peer_nonce', 'network_credit_socket']:
        if name not in radio.NAMES:
            radio.NAMES.append(name)
    stm.ELF = ROOT/'experiments/pico-bridge/images/stm32-baud-fixture.elf'
    result = {}
    for board, project, serial in [('A', 'Debugger-SiW917_AccessPoint', '440332408'),
                                    ('B', 'Debugger-SiW917_station', '440332624')]:
        elf = ROOT/f'experiments/pico-bridge/images/board-{board}-pico.out'
        manifest = json.loads((ROOT/'experiments/pico-bridge/build-manifest.json').read_text())
        if hashlib.sha256(elf.read_bytes()).hexdigest() != manifest['images'][elf.name]:
            raise RuntimeError('ELF identity mismatch: '+str(elf))
        result[board] = radio.capture(project, serial, elf_path=elf)
    result['STM32'] = stm.capture()
    if not all(result[b]['read_ok'] for b in ['A', 'B', 'STM32']):
        raise RuntimeError('Could not capture all matching radio/STM32 counters')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['ports', 'release', 'reset', 'boot', 'fixture', 'status', 'policy', 'latency', 'endurance'])
    parser.add_argument('--scope', help='Explicit diagnostic DATA path label; does not change firmware')
    parser.add_argument('--baud', type=int, choices=RATES, default=115200)
    parser.add_argument('--seconds', type=float, default=600)
    parser.add_argument('--rate', type=int)
    parser.add_argument('--report', type=Path)
    parser.add_argument('--capture-diagnostics', action='store_true', help='Read matching radio/STM32 counters only outside timed traffic')
    parser.add_argument('--allow-delay', action='store_true', help='Allow slower transfer and extra drain time; still require exact delivery')
    args = parser.parse_args()
    if args.command == 'policy':
        policy(args.report or ROOT/'docs'/f'pico-demo-policy-{datetime.datetime.now().strftime("%Y%m%d-%H%M%S")}.json')
        return
    if args.command == 'ports':
        for name, path in [('Pico data', DATA), ('Pico status', STATUS), ('STM32 fixture', FIXTURE)]:
            print(f'{name}: {path.resolve() if path.exists() else "MISSING"}')
        return
    if args.command == 'release':
        release()
        return
    if args.command in ('reset', 'boot'):
        target_control(args.command)
        return
    if args.command == 'fixture':
        fixture(args.baud)
        return
    if args.command == 'status':
        with serial_module().Serial(str(STATUS), 115200, timeout=.05, exclusive=True) as console:
            print(json.dumps(state(console), indent=2))
        return
    rate = args.rate if args.rate is not None else args.baud // 10
    if args.seconds <= 0 or not 0 < rate <= args.baud // 10:
        parser.error('Duration must be positive; rate must be between1 and baud/10')
    report_path = args.report or ROOT/'docs'/f'pico-demo-{args.command}-{datetime.datetime.now().strftime("%Y%m%d-%H%M%S")}.json'
    report = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  baud=args.baud, format='8N1', path=args.scope or 'USB Pico A Wi-Fi B UART STM32 echo and return',
                  image_manifest=json.loads((ROOT/'experiments/pico-bridge/build-manifest.json').read_text()),
                  passed=False)
    port = None
    console = None
    log_stop = threading.Event()
    log_thread = None
    bridge_logs = []
    try:
        port, applied, transition = open_data(args.baud, write_timeout=30 if args.allow_delay else 3)
        # USB open can toggle reset; configure the fixture after that transition.
        fixture(args.baud)
        time.sleep(.3)
        transition += port.read(port.in_waiting).hex()
        report.update(applied=applied, startup_transition_hex=transition)
        if args.capture_diagnostics:
            report['counters_before'] = capture_counters()
        console = serial_module().Serial(str(STATUS), 115200, timeout=.05, exclusive=True)
        report['Pico_before'] = state(console, args.baud)
        def collect_logs():
            while not log_stop.is_set():
                try:
                    line = console.readline().decode('ascii', 'replace').strip()
                    if line:
                        bridge_logs.append(line)
                except Exception as exc:
                    bridge_logs.append('STATUS PORT ERROR: ' + str(exc))
                    break
        log_thread = threading.Thread(target=collect_logs)
        log_thread.start()
        report.update(latency(port) if args.command == 'latency' else endurance(port, args.seconds, rate, args.allow_delay))
        if args.capture_diagnostics:
            report['counters_after'] = capture_counters()
    except Exception as exc:
        report['error'] = f'{type(exc).__name__}: {exc}'
    finally:
        log_stop.set()
        if log_thread:
            log_thread.join(timeout=2)
        if console:
            try:
                console.write(b'STATUS\n')
                deadline = time.monotonic()+1
                while time.monotonic() < deadline:
                    line = console.readline().decode('ascii', 'replace').strip()
                    if line:
                        bridge_logs.append(line)
                    if line.startswith('STATUS '):
                        report['Pico_after'] = dict(item.split('=', 1) for item in line.split()[1:] if '=' in item)
                if 'Pico_before' in report and 'Pico_after' in report:
                    report['bridge_error_increment'] = int(report['Pico_after']['errors'])-int(report['Pico_before']['errors'])
                    if report['bridge_error_increment']:
                        report['passed'] = False
                else:
                    report['passed'] = False
                    report.setdefault('error', 'Missing final Pico status')
            except Exception as exc:
                report['passed'] = False
                report['status_error'] = str(exc)
            console.close()
        report['bridge_logs'] = bridge_logs
        if port:
            port.close()
        report['finished_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(report, indent=2)+'\n')
    print(('PASS' if report['passed'] else 'FAIL') + ' report: ' + str(report_path))
    if not report['passed']:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
