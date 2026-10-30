#!/usr/bin/env python3
"""Paced full-duplex echo stream with per-chunk round-trip latency.

Writes a deterministic stream at --rate bytes/s through the Pico data port
(echoed by the STM32 fixture) and records, for every written chunk, the time
from write to the moment its last byte returned. Requires exact bytes.
"""
import argparse, bisect, datetime, hashlib, importlib.util, json, math, statistics, sys, threading, time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
spec = importlib.util.spec_from_file_location('demo', ROOT/'docs/demo-support.py')
demo = importlib.util.module_from_spec(spec); spec.loader.exec_module(demo)


def run(port, seconds, rate, chunk, drain):
    total = int(seconds*rate)
    src = demo.Stream()
    data = src.read(total)
    ends, times = [], []          # cumulative end offset of each chunk, write time
    lat, got = [], bytearray()
    state = dict(sent=0, error=None)
    start = time.monotonic()

    def writer():
        try:
            off = 0
            while off < total:
                due = start + off/rate
                d = due - time.monotonic()
                if d > 0: time.sleep(d)
                n = min(chunk, total-off)
                t = time.monotonic()
                port.write(data[off:off+n])
                off += n
                times.append(t); ends.append(off); state['sent'] = off
        except Exception as e:
            state['error'] = f'writer {type(e).__name__}: {e}'

    w = threading.Thread(target=writer); w.start()
    idx, last_rx = 0, time.monotonic()
    gap_max, gaps = 0.0, []
    while len(got) < total:
        b = port.read(65536)
        now = time.monotonic()
        if b:
            if now-last_rx > gap_max: gap_max = now-last_rx
            if now-last_rx > 0.2 and len(got): gaps.append((round(last_rx-start, 3), round(now-last_rx, 3)))
            last_rx = now
            got.extend(b)
            while idx < len(ends) and ends[idx] <= len(got):
                lat.append((now-times[idx])*1000); idx += 1
        elif now-last_rx > drain or state['error']:
            break
        if now-start > seconds*1.5+drain+30:
            state['error'] = state['error'] or 'overall deadline'
            break
    w.join(timeout=35)
    elapsed = time.monotonic()-start
    exact = bytes(got) == data
    mismatch = None
    if not exact:
        mismatch = next((i for i, (a, b) in enumerate(zip(got, data)) if a != b), min(len(got), total))
    s = sorted(lat)
    pct = lambda p: s[min(len(s)-1, math.ceil(p*len(s))-1)] if s else None
    return dict(requested_seconds=seconds, offered_Bps=rate, chunk_bytes=chunk,
                expected_bytes=total, sent_bytes=state['sent'], received_bytes=len(got),
                exact_bytes_passed=exact, first_mismatch=mismatch,
                sha256_sent=hashlib.sha256(data).hexdigest(), sha256_received=hashlib.sha256(bytes(got)).hexdigest(),
                elapsed_seconds=round(elapsed, 3), achieved_Bps=round(len(got)/elapsed, 1),
                writer_error=state['error'],
                latency_ms=dict(samples=len(s), median=pct(.5), p90=pct(.9), p99=pct(.99), p999=pct(.999),
                                max=s[-1] if s else None, over_100ms=sum(x > 100 for x in s),
                                over_250ms=sum(x > 250 for x in s), over_500ms=sum(x > 500 for x in s)),
                rx_gap_max_s=round(gap_max, 3), rx_gaps_over_200ms=gaps[:50])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--baud', type=int, default=921600)
    ap.add_argument('--seconds', type=float, default=60)
    ap.add_argument('--rate', type=int)
    ap.add_argument('--chunk', type=int, default=256)
    ap.add_argument('--drain', type=float, default=15)
    ap.add_argument('--report', type=Path, required=True)
    a = ap.parse_args()
    rate = a.rate or a.baud//10
    rep = dict(started_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), baud=a.baud,
               image_manifest_version=json.loads((ROOT/'experiments/pico-bridge/build-manifest.json').read_text()).get('version'))
    port = None
    try:
        port, applied, transition = demo.open_data(a.baud, write_timeout=30)
        demo.fixture(a.baud); time.sleep(.3); port.read(port.in_waiting)
        with demo.serial_module().Serial(str(demo.STATUS), 115200, timeout=.05, exclusive=True) as c:
            before = demo.state(c, a.baud)
        rep.update(run(port, a.seconds, rate, a.chunk, a.drain))
        with demo.serial_module().Serial(str(demo.STATUS), 115200, timeout=.05, exclusive=True) as c:
            after = demo.state(c, a.baud)
        rep['pico_error_increment'] = int(after['errors'])-int(before['errors'])
        rep['passed'] = rep['exact_bytes_passed'] and rep['pico_error_increment'] == 0
    except Exception as e:
        rep['error'] = f'{type(e).__name__}: {e}'; rep['passed'] = False
    finally:
        if port: port.close()
        rep['finished_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        a.report.parent.mkdir(parents=True, exist_ok=True)
        a.report.write_text(json.dumps(rep, indent=2)+'\n')
    summary = {k: rep.get(k) for k in ['passed', 'received_bytes', 'expected_bytes', 'elapsed_seconds', 'achieved_Bps', 'latency_ms', 'rx_gap_max_s', 'error']}
    print(json.dumps(summary, indent=1))
    sys.exit(0 if rep['passed'] else 1)


if __name__ == '__main__':
    main()
