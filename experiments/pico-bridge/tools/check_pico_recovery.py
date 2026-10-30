"""Explicitly requested radio-reset qualification on one open Pico COM handle."""
import datetime
import argparse
import importlib.util
import json
from pathlib import Path
import subprocess
import time
import serial

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('features', ROOT/'tools/check_pico_features.py')
f = importlib.util.module_from_spec(spec)
spec.loader.exec_module(f)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cycles',type=int,default=3,choices=[1,2,3])
    parser.add_argument('--report',type=Path,default=ROOT/'qualification-2026-10-28/final-recovery.json')
    args=parser.parse_args()
    report = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
        image_manifest=json.loads((ROOT/'build-manifest.json').read_text()),
        scope=f'{args.cycles} MCU resets per radio; Pico USB/data COM stays open; no byte preservation during outages',
        cycles=[], passed=False, COM_open_count=1)
    data = console = fixture = None
    try:
        path, status = f.identify()
        console = serial.Serial(status, 115200, timeout=.05, exclusive=True)
        data = f.open_data(path)
        fixture = serial.Serial('/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_066CFF535767864967152953-if02', 115200, timeout=2, exclusive=True)
        f.status_wait(console, baud=115200, parity='N', dtr=False, rts=False, timeout=45)
        f.fixture_baud(fixture, 115200)
        time.sleep(.3)
        report['startup_hex'] = data.read(data.in_waiting).hex()
        report['baseline_echo'] = f.echo(data, 256)
        fd = data.fileno()
        for cycle, (board, sn) in enumerate([('B', '440332624'), ('A', '440332408')]*args.cycles):
            started = time.monotonic()
            reset = subprocess.run(['/home/ghost/.silabs/slt/installs/archive/commander/commander',
                'device', 'reset', '--device', 'SiWG917M111MGTBA', '--serialno', sn],
                capture_output=True, text=True, timeout=20)
            row = dict(board=board, reset_returncode=reset.returncode,
                       reset_log=reset.stdout+reset.stderr, probes=0, discarded_outage_bytes=0)
            if reset.returncode:
                raise RuntimeError('Radio reset failed: '+row['reset_log'])
            time.sleep(.3)
            deadline = started+75
            while time.monotonic() < deadline:
                try:
                    state = f.status_wait(console, baud=115200, parity='N', dtr=False, rts=False, timeout=1)
                except TimeoutError:
                    continue
                if time.monotonic()-started < 2:
                    time.sleep(.2)
                    continue
                # Stale outage bytes are outside the integrity-test interval.
                row['discarded_outage_bytes'] += len(data.read(data.in_waiting))
                row['probes'] += 1
                payload = f'RECOVER_{cycle}_{row["probes"]:04d}\n'.encode()
                data.write(payload)
                answer = f.exact_read(data, len(payload), .6)
                if answer == payload:
                    row['recovery_seconds'] = round(time.monotonic()-started, 3)
                    row['state'] = state
                    time.sleep(.15)
                    row['discarded_outage_bytes'] += len(data.read(data.in_waiting))
                    row['post_recovery_exact_echo'] = f.echo(data, 4096)
                    row['same_COM_handle'] = data.fileno() == fd
                    row['passed'] = row['same_COM_handle'] and row['post_recovery_exact_echo']['passed']
                    report['cycles'].append(row)
                    print(f'PASS reset {board} cycle{cycle//2+1}: {row["recovery_seconds"]}s; same COM;4096 exact bytes', flush=True)
                    break
                row['discarded_outage_bytes'] += len(answer)
                time.sleep(.2)
            else:
                report['cycles'].append(row)
                raise TimeoutError(f'Radio {board} did not recover within75seconds')
        report['passed'] = all(c['passed'] for c in report['cycles']) and len(report['cycles']) == 2*args.cycles
    except Exception as exc:
        report['error'] = type(exc).__name__+': '+str(exc)
        print(report['error'], flush=True)
    finally:
        if data:
            try:
                data.dtr=False
                data.rts=False
            except Exception:
                pass
            data.close()
        if console:
            console.close()
        if fixture:
            fixture.close()
        report['status_lines'] = f.status_lines
        report['finished_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        args.report.write_text(json.dumps(report, indent=2)+'\n')
    raise SystemExit(0 if report['passed'] else 1)


if __name__ == '__main__':
    main()
