#!/usr/bin/env python3
"""Streaming long-duration exact-byte echo test (constant memory).

Writes a deterministic stream at --rate bytes/s through the Pico data port,
the STM32 fixture echoes it, and every returned byte is compared on arrival.
The first wrong/missing byte ends the run. Progress JSON is rewritten every
minute so a run can be inspected while it continues.
"""
import argparse, collections, datetime, hashlib, importlib.util, json, math, random, threading, time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
spec = importlib.util.spec_from_file_location('demo', ROOT/'docs/demo-support.py')
demo = importlib.util.module_from_spec(spec); spec.loader.exec_module(demo)


class Source:
    """Deterministic byte stream; two instances produce identical bytes."""
    def __init__(self):
        self.rng = random.Random(917); self.buf = b''
    def read(self, n):
        out = bytearray()
        while n:
            if not self.buf: self.buf = self.rng.randbytes(65536)
            take = min(n, len(self.buf)); out += self.buf[:take]; self.buf = self.buf[take:]; n -= take
        return bytes(out)


def now_utc(): return datetime.datetime.now(datetime.timezone.utc).isoformat()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--baud', type=int, default=115200)
    ap.add_argument('--seconds', type=float, default=86400)
    ap.add_argument('--rate', type=int)
    ap.add_argument('--chunk', type=int, default=256)
    ap.add_argument('--drain', type=float, default=60)
    ap.add_argument('--report', type=Path, required=True)
    ap.add_argument('--no-fixture', action='store_true', help='Fixture already at --baud (e.g. reset default 921600)')
    a = ap.parse_args()
    rate = a.rate or a.baud // 10
    total = int(a.seconds * rate)
    rep = dict(started_utc=now_utc(), baud=a.baud, format='8N1', offered_Bps=rate, requested_seconds=a.seconds,
               expected_bytes_each_direction=total, path='PC USB -> Pico -> A -> Wi-Fi -> B -> STM32 echo and back',
               image_manifest=json.loads((ROOT/'experiments/pico-bridge/build-manifest.json').read_text()),
               state='running', passed=False)
    st = dict(sent=0, received=0, mismatch=None, writer_error=None, done=False)
    sent_hash, recv_hash = hashlib.sha256(), hashlib.sha256()
    pending = collections.deque()          # (end_offset, write_time)
    lat_all = [0]*12                       # histogram buckets, ms: <25,50,100,250,500,1000,2000,3000,5000,10000,30000,more
    edges = [25, 50, 100, 250, 500, 1000, 2000, 3000, 5000, 10000, 30000]
    minute = dict(lat=[], gap=0.0)
    gaps = []
    lock = threading.Lock()

    def save(final=False):
        with lock:
            s = sorted(minute['lat']); n = len(s)
            row = dict(updated_utc=now_utc(), elapsed_s=round(time.monotonic()-t0, 1),
                       sent_bytes=st['sent'], received_bytes=st['received'],
                       backlog_bytes=st['sent']-st['received'],
                       last_minute=dict(samples=n, median_ms=s[n//2] if n else None,
                                        p99_ms=s[min(n-1, math.ceil(.99*n)-1)] if n else None,
                                        max_ms=s[-1] if n else None, rx_gap_max_s=round(minute['gap'], 3)),
                       latency_histogram_ms={f'<{e}': c for e, c in zip(edges, lat_all)} | {'>=30000': lat_all[-1]},
                       latency_max_ms=rep.get('latency_max_ms', 0), rx_gaps_over_1s=gaps[-50:],
                       first_mismatch=st['mismatch'], writer_error=st['writer_error'])
            minute['lat'] = []; minute['gap'] = 0.0
        rep['progress'] = row
        if final:
            rep.update(state='finished', finished_utc=now_utc(), sha256_sent=sent_hash.hexdigest(),
                       sha256_received=recv_hash.hexdigest())
        a.report.write_text(json.dumps(rep, indent=2)+'\n')

    port, applied, _ = demo.open_data(a.baud, write_timeout=120)
    attempt = -1
    for attempt in range(0 if a.no_fixture else 6):  # bench fixture control port can be slow after an open
        try: demo.fixture(a.baud); break
        except RuntimeError:
            if attempt == 5: raise
            time.sleep(1)
    rep['fixture_attempts'] = attempt + 1
    time.sleep(.3); port.read(port.in_waiting)
    rep['applied'] = applied
    t0 = time.monotonic()

    def writer():
        src = Source()
        try:
            while st['sent'] < total and not st['done']:
                due = t0 + st['sent']/rate
                d = due - time.monotonic()
                if d > 0: time.sleep(min(d, 1))
                if time.monotonic() < due: continue
                n = min(a.chunk, total-st['sent'])
                block = src.read(n); tw = time.monotonic()
                port.write(block); sent_hash.update(block)
                with lock: st['sent'] += n; pending.append((st['sent'], tw))
        except Exception as e:
            st['writer_error'] = f'{type(e).__name__}: {e}'

    w = threading.Thread(target=writer, daemon=True); w.start()
    expect = Source(); last_rx = time.monotonic(); next_save = t0 + 60
    try:
        while st['received'] < total:
            b = port.read(65536); t = time.monotonic()
            if b:
                want = expect.read(len(b))
                if b != want:
                    i = next(k for k in range(len(b)) if b[k] != want[k])
                    st['mismatch'] = dict(offset=st['received']+i, got=b[i:i+16].hex(), expected=want[i:i+16].hex(),
                                          elapsed_s=round(t-t0, 3))
                    break
                recv_hash.update(b)
                gap = t-last_rx
                with lock:
                    if gap > minute['gap']: minute['gap'] = gap
                    if gap > 1: gaps.append((round(last_rx-t0, 1), round(gap, 2)))
                    st['received'] += len(b)
                    while pending and pending[0][0] <= st['received']:
                        ms = (t-pending.popleft()[1])*1000
                        minute['lat'].append(ms)
                        lat_all[next((k for k, e in enumerate(edges) if ms < e), len(edges))] += 1
                        if ms > rep.get('latency_max_ms', 0): rep['latency_max_ms'] = round(ms, 1)
                last_rx = t
            elif t-last_rx > a.drain or st['writer_error']:
                break
            if t >= next_save: save(); next_save += 60
    finally:
        st['done'] = True
        w.join(timeout=5)
        port.close()
        rep['exact_bytes_passed'] = st['mismatch'] is None and st['received'] == total and st['sent'] == total
        try:
            with demo.serial_module().Serial(str(demo.STATUS), 115200, timeout=.05, exclusive=True) as c:
                rep['Pico_after'] = demo.state(c, timeout=10)
        except Exception as e:
            rep['Pico_after_error'] = str(e)
        rep['passed'] = rep['exact_bytes_passed'] and not st['writer_error']
        save(final=True)
    print(json.dumps({k: rep.get(k) for k in ['passed', 'exact_bytes_passed', 'latency_max_ms']} | {'progress': rep['progress']}, indent=1))


if __name__ == '__main__':
    main()
