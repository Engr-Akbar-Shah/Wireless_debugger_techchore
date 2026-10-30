#!/usr/bin/env python3
"""Read live 32-bit values from radio A/B by C expression (debug info from the ELF).

usage: probe.py A|B expr [expr...]   e.g. probe.py B network_udp.retransmits bridge_stop_reason
Reads only (J-Link mem32); the target keeps running.
"""
import re, subprocess, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GDB = '/home/ghost/.silabs/slt/installs/conan/p/gcc-a469753773d4f2/p/bin/arm-none-eabi-gdb'
BOARDS = {'A': ('440332408', ROOT/'images/board-A-pico.out'), 'B': ('440332624', ROOT/'images/board-B-pico.out')}


def addresses(elf, exprs):
    args = [GDB, '-batch', '-nx', str(elf)]
    for e in exprs:
        args += ['-ex', f'printf "ADDR %s 0x%x\\n", "{e}", &({e})']
    out = subprocess.run(args, capture_output=True, text=True).stdout
    found = dict(re.findall(r'ADDR (\S+) (0x[0-9a-f]+)', out))
    return {e: int(found[e], 16) for e in exprs if e in found}


def read(board, exprs):
    serial, elf = BOARDS[board]
    where = addresses(elf, exprs)
    script = 'connect\n' + ''.join(f'mem32 0x{a:X}, 1\n' for a in where.values()) + 'exit\n'
    with tempfile.TemporaryDirectory() as d:
        p = Path(d)/'r.jlink'; p.write_text(script)
        out = subprocess.run(['/opt/SEGGER/JLink/JLinkExe', '-device', 'Cortex-M4', '-if', 'SWD', '-speed', '1000',
                              '-SelectEmuBySN', serial, '-CommanderScript', str(p)],
                             capture_output=True, text=True, timeout=30).stdout
    mem = {int(a, 16): int(v, 16) for a, v in re.findall(r'(?im)^([0-9a-f]{8})\s*=\s*([0-9a-f]{8})', out)}
    return {e: mem.get(a) for e, a in where.items()} | {e: None for e in exprs if e not in where}


if __name__ == '__main__':
    for k, v in read(sys.argv[1], sys.argv[2:]).items():
        print(f'{k:40s} {v if v is None else f"{v} (0x{v:08x})"}')
