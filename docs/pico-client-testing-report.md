# Wireless UART engineering test report

28 October 2026 | Pico RP2040 and SiWx917 prototype | Linux

The prototype replaces a wired UART connection with a point-to-point wireless link. USB baud selections and reset and boot signals now reach the target through the Pico and two radios. These functions passed the recorded checks. Fresh sustained-load checks failed at both required rates. The setup is suitable for a limited functional demonstration; it is not ready for full acceptance. This report distinguishes functional results from endurance qualification.

## Hardware and data path

![Wireless UART data path](report-assets/pico-demo-data-path.png)

- Laptop running Linux, with nRF Connect Serial Terminal 1.6.5 for the text demonstration.
- Raspberry Pi Pico RP2040 provides the USB serial interface. Its two COM ports are payload and diagnostics.
- Two SiWx917 development boards provide a direct Wi-Fi TCP link. A is the laptop side; B is the target side. No router is used.
- NUCLEO-L452RE MB1136 Rev C provides a known UART echo fixture. Its separate ST-LINK port selects the fixture baud and exposes counters.
- A and B USB cables provide power and programming or diagnostic access. Application payload travels through the Pico USB connection.

### Jumper connections

| From | To |
| --- | --- |
| Pico GP0 TX pin 1 | A GPIO29 RX EXP11 |
| Pico GP1 RX pin 2 | A GPIO30 TX EXP13 |
| Pico GND pin 3 | A GND EXP1 |
| B GPIO30 TX EXP13 | Nucleo D2 PA10 RX CN9 pin 3 |
| B GPIO29 RX EXP11 | Nucleo D8 PA9 TX CN5 pin 1 |
| B GND | Nucleo marked GND |
| B GPIO25 EXP3 | Nucleo NRST CN6 pin 3 |
| B GPIO26 EXP5 | Nucleo BOOT0 CN7 pin 7 |

All jumper signals use 3.3 V logic. There is no power jumper between the boards. The Pico to A transport is fixed at 2 Mbaud; it carries framed payload and control messages. B applies the selected nominal target UART setting. Its driver uses a documented 0.75 percent adjustment; the fixture uses its matching clock adjustment. Requested and driver values are recorded separately in the test evidence.

<!-- pagebreak -->

## Requirements and measured results

| Requirement | Result |
| --- | --- |
| Buffer full or short idle batching | Implemented. Pico batches up to 256 bytes or 2 ms of USB idle. A forwards the bounded PC batches. B uses up to 1440 bytes or about 4 ms of UART idle. Short exact echoes passed. |
| Wi-Fi power save disabled | High-performance firmware policy is implemented. Software policy readback is recorded separately. No current consumption measurement is claimed. |
| Buffering with no byte loss | UART DMA, bounded queues and transport acknowledgements are implemented. Fresh sustained-load checks failed at 115200 and 921600. See the timed results below. |
| Reconnect without reopening COM | PASS. One reset of each radio recovered on the same open Pico data handle. Each recovery was followed by a 4096-byte exact echo. |
| Laptop baud selection reaches target | PASS. 9600, 57600, 115200, 230400, 460800 and 921600 were selected on one open COM handle. Each selection passed 4096 exact bytes in each direction. |
| USB DTR and RTS control reset and boot | PASS for control and ROM entry. Actual USB signals entered the STM32 ROM at 115200 8E1, read chip ID 0462 and restored application echo. |
| Wireless UART flash programming | NOT QUALIFIED on this Pico path. STM32CubeProgrammer activation failed during a read-only connection attempt. No erase or write followed. The application was restored and echoed correctly. |

A serial bridge changes the target-side interface baud. The target application must independently use that same baud. For these checks, the fixture was explicitly configured through its separate ST-LINK control port.

### Timed data integrity

An earlier run on the restored 3.9.1 bundle passed 600 seconds at 115200: 6912000 exact bytes each way at 11520 bytes/s, with matching hashes and zero measured error increments. A fresh confirmation on the same image hashes then failed after about 95 seconds. Its first mismatch was at byte offset 1000543. This inconsistent result prevents a reliable sustained-115200 claim.

The later run also exposed a diagnostic-tool issue: A counter reads used a stale build ELF after restoration. Those A counters are excluded from conclusions. The host mismatch remains valid. The helper now verifies the archived ELF against the image manifest before reading counters.

- At 921600 8N1, nominal payload capacity is 92160 bytes/s per direction. The new Pico path did not sustain that offered load.
- A longer diagnostic run on the preceding MSS image also detected a byte mismatch and B return-buffer overflow. The STM32 received and echoed all bytes delivered to it with no recorded UART errors in that run.
- Pico counters showed extended waits for A acknowledgements while USB input remained available. The remaining throughput bottleneck has not been isolated. Electrical noise has not been established as the cause.
- A preserved earlier implementation passed one hour at full nominal 921600 capacity. That result belongs to its recorded images and does not qualify this changed Pico implementation.

<!-- pagebreak -->

## Test methods and acceptance limits

### Data integrity and supported settings

The host sends deterministic binary data, compares every echoed byte and records byte counts and SHA256 hashes. A sustained pass also requires the source to finish at the requested rate. A slow lossless transfer does not pass the full-capacity criterion. Diagnostic counters are read before and after qualifying timed traffic, outside its measurement interval.

The native software tests also passed. They cover configuration acknowledgements, binary framing, queue backpressure, idle batching, unsupported formats, timeout handling and actual A and B dispatch logic with mocked hardware.

### Reconnect timing

| Reset event | Recovery seconds |
| --- | --- |
| Radio B reset | 21.093 |
| Radio A reset | 34.231 |

Time is measured from the reset command to the first correct fresh reply. The same COM handle remained open. Bytes sent during a link outage are not guaranteed to survive. These checks simulate radio MCU resets; they are not a controlled RF attenuation test.

### Latency

50 exact echoes were measured for each message size at each rate. Results below are round-trip milliseconds, including USB, both radios and the STM32 echo.

| Bytes | 115200 median and max ms | 921600 median and max ms |
| --- | --- | --- |
| 1 | 19.1 / 112.0 | 17.0 / 90.6 |
| 32 | 24.3 / 961.1 | 19.5 / 93.2 |
| 256 | 53.9 / 148.0 | 38.1 / 155.0 |
| 1024 | 140.6 / 207.3 | 68.4 / 148.5 |

The reports also retain p95 values. No numerical acceptance limit was specified. The largest observed sample is not an absolute worst-case bound.

### Remaining acceptance work

- Neither 24-hour endurance test has passed. Ten minutes or one hour cannot establish a 24-hour guarantee.
- Range through one or two walls has no verified pass. Record distance, wall material and exact data results at each placement before making a range claim.
- Continuous full-capacity 921600 operation and standard flashing-tool compatibility remain open.
- nRF device discovery and equivalent Linux serial open and control behaviour were checked. The automated tests did not drive the nRF graphical interface.

nRF has no manual DTR and RTS buttons. Its demonstration uses a Linux command to release those signals after opening the data port. Baud forwarding itself is handled by firmware.

The supported presentation is ordinary text echo, deliberate baud selection, USB reset and ROM identification, and reconnect. Use the accompanying demonstration guide. Do not present high-speed endurance, 24-hour operation, wall range or standard flash programming as completed.

<!-- pagebreak -->

## Reproducibility and evidence

### Firmware identity

The tested bundle is recorded in `experiments/pico-bridge/build-manifest.json`. Every hardware result embeds its image manifest and SHA256 values. A is serial 440332408, B is 440332624, Pico is E66488C15F39C034 and ST-LINK is 066CFF535767864967152953.

The restored component labels are A 3.9.1, B 3.9.0, Pico 3.9.1 and STM32 fixture 3.8.0. Image hashes take precedence over version labels. Original images and prior results remain preserved.

| Evidence | File within qualification folder |
| --- | --- |
| Six baud selections and USB reset and ROM entry | final-features.json |
| Final 115200 timed result | restored-115200-600s.json |
| Earlier successful 115200 stage verification | final-2M-115200-full-600s-verification.json |
| Restored bundle reconnect on one COM handle | restored-recovery.json |
| Latency measurements | final-latency-115200.json and final-latency-921600.json |
| Experimental candidate programmer failure and restore | final-CubeProgrammer-readonly.json |
| High-rate offered-load failure | credit-scheduling-921600-30s.json |
| Earlier Pico-path high-rate mismatch | MSS-921600-full-600s.json |

The standard programmer attempt and extra six recovery cycles used the rejected A 3.9.2 candidate. They remain image-scoped. The restored A 3.9.1 has its own two-cycle recovery result; standard programmer compatibility remains unqualified.

The qualification folder is `experiments/pico-bridge/qualification-2026-10-28/`. The requirement checklist is `docs/client-requirements-status.md`. The demonstration instructions are `docs/pico-client-demo-guide.docx`.

### Interpretation

PASS applies only to the recorded hardware, images, settings, duration and measured behaviour. A short functional pass establishes that a feature works under the tested conditions. It does not establish endurance, maximum range or a guaranteed worst-case delay.
