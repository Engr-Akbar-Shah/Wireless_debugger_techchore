# Wireless UART bridge (Wireless debugger)

A drop-in replacement for a USB-to-serial cable: laptop USB → Raspberry Pi Pico → SiWx917 A (Wi-Fi AP) ~ Wi-Fi ~ SiWx917 B (station) → target UART (STM32). Baud changes on the laptop are forwarded to the target, and DTR/RTS drive the target's NRST/BOOT0, so the STM32 can be reset and flashed over Wi-Fi with STM32CubeProgrammer.

## Branches

| Branch | Contents |
| --- | --- |
| `siw917-ap` | Board A firmware (Wi-Fi access point "WD-v1", TCP server, Pico link) + prebuilt image |
| `siw917-station` | Board B firmware (Wi-Fi station, target UART, NRST/BOOT0) + prebuilt image |
| `pico` | Raspberry Pi Pico USB bridge firmware + prebuilt UF2 + native unit tests |
| `stm32-fixture` | NUCLEO-L452RE echo test fixture (STM32CubeIDE project) + prebuilt ELF |
| `docs` | Test report, requirement status, demo guides (also merged into `main` under `docs/`) |
| `scripts` | Test and demo scripts with a run guide |

## Firmware state (30 Oct 2026)

| Part | Version |
| --- | --- |
| Board A / Board B | 3.11.12-hb15s (radio protocol 6, TCP) |
| Pico | 3.12.1 (auto bootloader on 8E1, auto-reset DTR/RTS) |
| STM32 fixture | 3.8.0 (adjustable baud echo) |

See [docs/test-report-2026-10-30.md](docs/test-report-2026-10-30.md) for test results and limitations (24 h continuous 921600 is not yet qualified), and [docs/README.md](docs/README.md) for all documents.

## Wiring

| From | To |
| --- | --- |
| Pico GP0 (TX, pin 1) | A GPIO29 / EXP11 |
| Pico GP1 (RX, pin 2) | A GPIO30 / EXP13 |
| Pico GND (pin 3) | A GND / EXP1 |
| B GPIO30 / EXP13 (TX) | STM32 PA10 / D2 (RX) |
| B GPIO29 / EXP11 (RX) | STM32 PA9 / D8 (TX) |
| B GPIO25 / EXP3 | STM32 NRST / CN6.3 |
| B GPIO26 / EXP5 | STM32 BOOT0 / CN7.7 |
| B GND | STM32 GND |

Each board is powered from its own USB port. Do not connect Pico 3V3/VBUS to A.
