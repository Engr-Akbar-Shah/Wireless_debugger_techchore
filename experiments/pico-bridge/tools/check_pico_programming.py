"""Test standard STM32CubeProgrammer UART access through Pico USB/Wi-Fi.

Default is read-only. --program-fixture writes only this experiment's saved
fixture image. Boot/reset uses real USB DTR/RTS; no SWD programming fallback.
"""
import argparse
import datetime
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import subprocess
import threading
import time
import serial

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('features', ROOT/'tools/check_pico_features.py')
f = importlib.util.module_from_spec(spec)
spec.loader.exec_module(f)


_demo_spec = importlib.util.spec_from_file_location('demo', ROOT.parents[1]/'docs/demo-support.py')
demo = importlib.util.module_from_spec(_demo_spec)
_demo_spec.loader.exec_module(demo)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--program-fixture', action='store_true')
    parser.add_argument('--trace',action='store_true')
    parser.add_argument('--report-dir',type=Path,default=ROOT/'qualification-2026-10-29')
    args = parser.parse_args()
    args.report_dir.mkdir(parents=True,exist_ok=True)
    manifest = json.loads((ROOT/'build-manifest.json').read_text())
    image = ROOT/'images/stm32-baud-fixture.elf'
    digest = hashlib.sha256(image.read_bytes()).hexdigest()
    assert digest == manifest['images'][image.name]
    result = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                  image_manifest=manifest, image_sha256=digest,
                  program_requested=args.program_fixture, swd_programming_used=False,
                  control_path='USB DTR/RTS Pico UART A Wi-Fi B NRST/BOOT0', passed=False)
    data = console = fixture = None
    log_thread = None
    stop = threading.Event()
    lines = []
    try:
        path, status = f.identify()
        console = serial.Serial(status, 115200, timeout=.05, exclusive=True)
        data = f.open_data(path)
        f.status_wait(console, dtr=False, rts=False, timeout=40)
        # Client flow: no helper and no DTR/RTS options. CubeProgrammer opens
        # the port at 8E1, which the Pico turns into reset + BOOT0 + release.
        data.close()
        data=None
        cmd = ['STM32_Programmer_CLI', '-q', '-c', 'port='+path, 'br=115200',
               'P=EVEN', 'db=8', 'sb=1', 'fc=OFF']
        cmd += ['-d', str(image), '-v'] if args.program_fixture else ['-r32', '0x08000000', '16']
        result['command'] = cmd
        def collect():
            next_query=0
            while not stop.is_set():
                if time.monotonic()>=next_query:
                    console.write(b'STATUS\n')
                    next_query=time.monotonic()+1
                line=console.readline().decode('ascii', 'replace').strip()
                if line:
                    lines.append(line)
        log_thread=threading.Thread(target=collect)
        log_thread.start()
        started=time.monotonic()
        traced_cmd=cmd
        if args.trace:
            traced_cmd=['strace','-ff','-ttt','-s','128','-o',str(args.report_dir/'programmer-strace'),'-e','trace=openat,close,ioctl,read,write,poll,select,pselect6,ppoll']+cmd
        run=subprocess.run(traced_cmd, capture_output=True, text=True, timeout=150)
        result['elapsed_seconds']=round(time.monotonic()-started, 3)
        log=re.sub(r'\x1b\[[0-9;]*m', '', run.stdout+run.stderr)
        tag='programming' if args.program_fixture else 'CubeProgrammer-readonly'
        (args.report_dir/f'final-{tag}.log').write_text(log)
        result['programmer_returncode']=run.returncode
        result['download_verified']=args.program_fixture and run.returncode==0 and 'Download verified successfully' in log
        result['read_only_connected']=not args.program_fixture and run.returncode==0 and '08000000' in log
        stop.set()
        log_thread.join(timeout=2)
        log_thread=None
        print(log[-2500:], flush=True)
        if not (result['download_verified'] or result['read_only_connected']):
            raise RuntimeError('CubeProgrammer did not confirm the requested operation')
    except Exception as exc:
        result['error']=type(exc).__name__+': '+str(exc)
        print(result['error'], flush=True)
    finally:
        stop.set()
        if log_thread:
            log_thread.join(timeout=2)
        if data:
            data.close()
        if console:
            try:
                path, _=f.identify()
                data=f.open_data(path)
                fixture=serial.Serial('/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_066CFF535767864967152953-if02',115200,timeout=2,exclusive=True)
                result['application_restored']=f.restore_app(data,console,fixture)
            except Exception as exc:
                result['restore_error']=type(exc).__name__+': '+str(exc)
            finally:
                if data:
                    data.close()
                if fixture:
                    fixture.close()
                console.close()
        result['status_lines']=f.status_lines+lines
        result['passed']=(not result.get('error') and result.get('application_restored',{}).get('passed',False)
                          and (result.get('download_verified') or result.get('read_only_connected')))
        result['finished_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
        tag='programming' if args.program_fixture else 'CubeProgrammer-readonly'
        (args.report_dir/f'final-{tag}.json').write_text(json.dumps(result,indent=2)+'\n')
    raise SystemExit(0 if result['passed'] else 1)


if __name__=='__main__':
    main()
