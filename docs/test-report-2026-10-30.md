# Wireless UART Bridge — Test Report

30 October 2026

## Summary

All six functional requirements pass on the bench with zero data loss. Continuous full-load 921600 operation is the open item: it runs exactly for 5–17 minutes, then the Wi-Fi link drops.

- **Passed:** batching, power-save off, baud forwarding (9600–921600), reconnect on the same COM port, DTR/RTS reset, and wireless STM32 flashing with standard STM32CubeProgrammer.
- **Zero byte loss** in every test where the link stayed up, at 115200 and 921600.
- **Not passed:** 24 h continuous at 921600 (best run 17 min). 24 h at 115200 and the wall/range test were not run.

## Test setup

A laptop talks to an STM32 target through a Pico USB bridge and two SiWx917 Wi-Fi boards; the STM32 echoes every byte back.

| Part | Hardware | Firmware (30 Oct 2026) | Role |
| --- | --- | --- | --- |
| USB bridge | Raspberry Pi Pico (RP2040) | 3.12.1 | USB serial port on the laptop; forwards baud, DTR/RTS; 2 Mbaud UART to A |
| Radio A | SiWx917 BRD2605A (SN 440332408) | 3.11.12 | Wi-Fi access point "WD-v1", channel 6, TCP server |
| Radio B | SiWx917 BRD2605A (SN 440332624) | 3.11.12 | Wi-Fi station; target UART, NRST and BOOT0 pins |
| Target | NUCLEO-L452RE (STM32L452) | Echo fixture 3.8.0 | Echoes all received bytes; baud set via ST-LINK port |

- **Wiring:** Pico GP0/GP1/GND to A GPIO29/GPIO30/GND. B GPIO30/GPIO29 to STM32 PA10/PA9, B GPIO25 to NRST, B GPIO26 to BOOT0.
- **Serial format:** 8N1 at the selected baud (9600–921600), no flow control. STM32 bootloader uses 115200 8E1.
- **Wi-Fi:** 2.4 GHz, 20 MHz, power save off (high-performance profile), boards on the same office bench.

## Test method

Every test compares each returned byte with what was sent; one wrong or missing byte fails the test.

- **Data tests:** a script writes a fixed pseudo-random stream through the Pico port at the full baud rate (baud ÷ 10 bytes/s, both directions at once). The STM32 echoes it; the script checks bytes and SHA-256, and timestamps every 256-byte block to get round-trip delay.
- **Feature tests:** scripted serial-port actions (baud change, DTR/RTS, port open/close) with exact 4096-byte echoes after each step, plus STM32 bootloader GET\_ID.
- **Recovery:** each radio is reset while the laptop COM port stays open; time until exact data flows again.
- **Flashing:** STM32CubeProgrammer 2.21.0 CLI over the Pico port, download plus verify.
- Evidence files: `experiments/pico-bridge/claude-2026-10-30/` (one JSON per test).

## Results by requirement

| # | Requirement | Result | Evidence |
| --- | --- | --- | --- |
| 1 | Byte batching (buffer full or short idle) | Pass | Pico 256 B / 2 ms idle, A up to 1440 B / 6 ms, B 1440 B / 4 ms; idle 1-byte echo 19.5 ms |
| 2 | Wi-Fi power save disabled | Pass | Both radios read back high-performance profile (status 0, profile 0) |
| 3 | No byte loss at high baud | Pass while the link stays up | 921600: 27,648,000 bytes exact in 5 min; 115200: 3,456,000 bytes exact in 5 min. See 24 h status below |
| 4 | Auto reconnect without reopening COM | Pass | B reset: link back in 10.9 s; A reset: 27.7 s; same open port, 4096 bytes exact after each. Bytes sent during the outage are not kept |
| 5 | Laptop baud change applied on target side | Pass | 9600, 57600, 115200, 230400, 460800, 921600: 4096 bytes exact each, one open port |
| 6 | DTR/RTS to NRST/BOOT0, wireless flashing | Pass | Terminal open does not reset the STM32. STM32CubeProgrammer at 115200 8E1 entered the bootloader automatically (chip ID 0x462), programmed and verified in 19.6 s with no extra steps; application restarted on reopen at 8N1 |

DTR/RTS behaviour: equal lines (as any terminal sets them) leave the target running; RTS alone holds reset; DTR alone selects BOOT0. Opening the port at 8E1 selects the bootloader automatically.

## Acceptance tests

Short full-load runs are exact at both baud rates; the 24 h run at 921600 did not pass.

| Test | 115200 | 921600 |
| --- | --- | --- |
| Continuous full load, both directions | 300 s, 3,456,000 B exact | 300 s, 27,648,000 B exact (89.4 KB/s achieved) |
| 24 h continuous, zero loss | Not run | Failed: 3 valid attempts lasted 9–17 min; up to 77 MB exact before the drop |
| Round-trip delay, idle, 1 byte (median / max) | 19.4 / 139 ms | 19.5 / 105 ms |
| Round-trip delay, idle, 1024 bytes (median / max) | 126 / 298 ms | 51 / 70 ms |
| Round-trip delay under continuous full load (median / max) | 0.30 / 0.58 s | 0.62 / 2.45 s |
| Recovery after link drop (radio reset) | B 10.9 s, A 27.7 s | same mechanism |
| Range through one or two walls | Not run (excluded) | Not run (excluded) |

Why 921600 continuous fails: under nonstop load in both directions the Wi-Fi module's internal TCP stalls for 1–3 s after lost radio frames, data queues up, and eventually the connection is closed by the module or a watchdog. Each restart discards the bytes in flight. Fixes applied today (longer backpressure and link timeouts) moved the failure point but did not remove it.

## Limitations and next steps

- **921600 nonstop:** fine for bursty use (terminal, logs, flashing); not qualified for hours of continuous full load. Fix options: finish a faster-retransmit transport over UDP (in development), or add RTS/CTS flow control to the target.
- **Target flow control:** without RTS/CTS the STM32 cannot be paused. B buffers about 24 KB (0.26 s at 921600) during Wi-Fi stalls.
- **Delay:** a few tens of ms when idle, up to seconds under continuous full load. Data is delayed, not lost, while the link stays up.
- **8E1 applications:** opening the port at 8E1 always selects the STM32 bootloader, so a target application that itself uses even parity would be affected.
- **Not yet run:** 24 h at 115200 (recommended next) and the one/two-wall range test.
- Recovery times were measured earlier on 30 Oct on the same hardware; later firmware changes only lengthened timeouts.
