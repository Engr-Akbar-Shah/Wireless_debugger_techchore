# Test and demo scripts — run guide

Linux scripts used to test the wireless UART bridge (laptop → Pico → SiWx917 A ~ Wi-Fi ~ SiWx917 B → STM32 echo fixture). The folder layout matches the original workspace because the scripts find each other and the firmware ELFs by relative path; keep it as is.

## 1. Prerequisites

| Need | For |
| --- | --- |
| Linux, Python 3.10+, `pip install pyserial` | all scripts |
| All four boards on USB, wired as in the `main` branch README | all hardware tests |
| Pico firmware 3.12.1, A/B 3.11.12, STM32 fixture 3.8.0 | expected behaviour (see firmware branches) |
| STM32CubeProgrammer CLI (`STM32_Programmer_CLI` on PATH) | flashing test, STM32 diagnostics |
| SEGGER J-Link (`/opt/SEGGER/JLink/JLinkExe`) | radio counter reads (`--capture-diagnostics`, `probe.py`) |
| Simplicity Commander | radio reset in the reconnect test |

Close every serial terminal (nRF Serial Terminal, CubeProgrammer) on the Pico ports before running a script; the scripts open the ports exclusively. Run all commands from the repository root.

## 2. Bench-specific settings (edit for other hardware)

| Setting | File and line |
| --- | --- |
| Pico USB serial number (port names) | `docs/demo-support.py` (`PICO = …`) |
| ST-LINK serial number (fixture port) | `docs/demo-support.py` (`FIXTURE`), `experiments/pico-bridge/tools/check_pico_*.py`, `tools/capture_stm32_diagnostics.py` |
| J-Link serials of A (440332408) and B (440332624) | `docs/demo-support.py`, `tools/capture_bridge_diagnostics.py`, `experiments/pico-bridge/tools/check_pico_recovery.py`, `experiments/pico-bridge/bench/probe.py` |
| Simplicity Commander path | `experiments/pico-bridge/tools/check_pico_recovery.py` |
| `arm-none-eabi-gdb` path | `experiments/pico-bridge/bench/probe.py` (`GDB = …`) |

Find your own values with `ls -l /dev/serial/by-id/`.

## 3. Everyday commands (`docs/demo-support.py`)

```bash
python3 docs/demo-support.py ports                 # show Pico data/status and fixture ports
python3 docs/demo-support.py status                # bridge state (baud, DTR/RTS, link ready)
python3 docs/demo-support.py fixture --baud 921600 # set the STM32 echo fixture's baud
python3 docs/demo-support.py reset                 # reset the STM32 into its application over Wi-Fi
python3 docs/demo-support.py boot                  # reset the STM32 into its ROM bootloader over Wi-Fi
python3 docs/demo-support.py policy                # read back Wi-Fi power-save policy on A and B (J-Link)
```

## 4. Tests

| What | Command | Pass criterion |
| --- | --- | --- |
| Idle latency (1/32/256/1024 B, 50 samples each) | `python3 docs/demo-support.py latency --baud 921600 --report results/latency.json` | exact echoes; prints median/p95/max |
| Short full-rate endurance | `python3 docs/demo-support.py endurance --baud 921600 --seconds 300 --report results/endurance.json` | every byte exact, offered rate met (add `--allow-delay` to accept slower delivery) |
| Full-rate load with per-block latency | `python3 experiments/pico-bridge/bench/loadtest.py --baud 921600 --seconds 60 --report results/load.json` | exact bytes; reports latency median/p99/max under load |
| Long run (constant memory, progress every minute) | `python3 experiments/pico-bridge/bench/longrun.py --baud 115200 --seconds 86400 --report results/24h.json` | stops at the first wrong/missing byte; `--no-fixture` if the fixture is already at the baud |
| Baud matrix + bootloader ID + return to app | `python3 experiments/pico-bridge/tools/check_pico_features.py` | six rates exact, ROM GET_ID 0462; writes `experiments/pico-bridge/hardware-result.json` |
| Reconnect on the same open port | `python3 experiments/pico-bridge/tools/check_pico_recovery.py --cycles 1 --report results/reconnect.json` | B and A reset, 4096 bytes exact after each; prints recovery seconds |
| Wireless flashing with STM32CubeProgrammer | `python3 experiments/pico-bridge/tools/check_pico_programming.py --program-fixture --report-dir results/flash` | "Download verified successfully", app echoes afterwards |

Create the folder first: `mkdir -p results`. Run a long test in the background and watch it:

```bash
nohup python3 experiments/pico-bridge/bench/longrun.py --baud 115200 --seconds 86400 --report results/24h.json > results/24h.log 2>&1 &
python3 -c "import json;p=json.load(open('results/24h.json'))['progress'];print(p['elapsed_s'],'s',p['received_bytes'],'B, mismatch:',p['first_mismatch'])"
```

## 5. Manual check with a serial terminal

1. Fixture port (ST-LINK VCOM, 115200): send `BAUD 115200`.
2. Pico data port (interface 0) at 115200 8N1, flow control off: type text, it echoes. No DTR/RTS step is needed.
3. Pico status port (interface 2): send `STATUS`.

Flashing by hand: open STM32CubeProgrammer, UART, Pico data port, 115200, parity Even, Connect. The STM32 enters its bootloader automatically; after flashing, reopen a terminal at 8N1 and the application runs.

## 6. Diagnostics

`python3 experiments/pico-bridge/bench/probe.py A g_tcp_socket bridge_stop_reason` reads live firmware variables over J-Link, using `experiments/pico-bridge/images/board-*.out` for addresses. These ELFs must match the flashed firmware.
