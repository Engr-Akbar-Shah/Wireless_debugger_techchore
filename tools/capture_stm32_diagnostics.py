#!/usr/bin/env python3
"""Read STM32 fixture counters outside timed transfers; no reset or writes."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
ELF = ROOT / 'test-fixtures/Debug/test-fixtures.elf'
NAMES = ['SystemCoreClock', 'uart_errors', 'uart_overrun_errors',
         'uart_framing_errors', 'uart_noise_errors', 'queue_overflows',
         'target_dma_errors', 'target_dma_received_bytes',
         'target_dma_transmitted_bytes', 'target_dma_rx_hash',
         'target_dma_tx_hash', 'target_dma_tx_errors', 'target_dma_tx_queue_peak']


def capture():
    symbols = {}
    for line in subprocess.check_output(['arm-none-eabi-nm', '-n', str(ELF)], text=True).splitlines():
        fields = line.split()
        if len(fields) == 3:
            symbols[fields[2]] = int(fields[0], 16)
    selected = {name: symbols[name] for name in NAMES}
    command = ['STM32_Programmer_CLI', '-c', 'port=SWD',
               'sn=066CFF535767864967152953', 'mode=HOTPLUG']
    for address in selected.values():
        command += ['-r32', hex(address), '4']
    command += ['-r32', '0x40013800', '16']
    run = subprocess.run(command, text=True, capture_output=True, timeout=30)
    output = re.sub(r'\x1b\[[0-9;]*m', '', run.stdout + run.stderr)
    memory = {}
    for match in re.finditer(r'(?m)^0x([0-9A-Fa-f]{8})\s*:\s*([0-9A-Fa-f ]+)$', output):
        address = int(match.group(1), 16)
        for index, value in enumerate(match.group(2).split()):
            memory[address + 4 * index] = int(value, 16)
    result = {'timestamp_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
              'elf_sha256': hashlib.sha256(ELF.read_bytes()).hexdigest(),
              'current': {name: memory.get(address) for name, address in selected.items()},
              'usart1': {name: memory.get(0x40013800 + offset)
                         for name, offset in [('cr1', 0), ('cr2', 4), ('cr3', 8), ('brr', 12)]},
              'read_ok': run.returncode == 0 and all(address in memory for address in selected.values()),
              'scope': 'Read outside timed transfer; counters since STM32 application startup.'}
    if not result['read_ok']:
        result['error'] = output[-3000:]
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, required=True)
    args = parser.parse_args()
    result = capture()
    args.report.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
    raise SystemExit(0 if result['read_ok'] else 1)
