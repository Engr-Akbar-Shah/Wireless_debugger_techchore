# Requirements checklist — source of truth

## PAUSED29 October2026 by explicit user request

No hardware test or flash is running. **Current A/B3.10.1 radio protocol6, Pico3.9.7, STM32 fixture3.8.0.** Reliable payloadTCP5005 now uses an independent cumulative-credit UDP5006 channel. Both radio flashes completed. Its short full-rate test was aborted by a Pico USB/serial disconnect, so current throughput is not qualified. Resume from [the saved checkpoint](RESUME-WIRELESS-UART.md); [machine-readable state and source/image hashes](../experiments/pico-bridge/qualification-2026-10-29/paused-checkpoint.json). Do not start more work until the user resumes.

The3.9.6 standard wireless programmer download/verify passed and is image-scoped. Forward-only radio full-rate30s passed; separate directional TCP3.10.0 did not fix full-duplex stalls and is rejected. Vendor full-SoC reset was rejected after startup stalls; original core recovery restored. Client DOCX reports are still28October versions and need updating after final qualification.

Updated28 October2026. Read before any hardware test or flash. Do not repeat completed tests without a relevant change or explicit user request. Preserve each result's firmware identity.

## Current bench status28 October2026

- Current hardware runs the isolated Pico experiment A/B3.9.0, matching Pico firmware and adjustable STM32 fixture3.8.0. USB payload goes PC→Pico→UART A→Wi-Fi TCP B→UART STM32. A USB is power/debug access, not the payload path.
- **COMPLETE, functional:** all six supported baud rates with exact short binary echoes on one open COM handle; USB DTR/RTS→ROM GET_ID0462; application restoration and exact echo. [Frozen pass](../experiments/pico-bridge/2026-10-28-pico-functional-pass.json). Do not repeat without a relevant change or requested live demonstration.
- Current Pico images have not completed sustained full-capacity,24-hour, latency/reconnect or wall-range qualification. Preserved original-image endurance passes below remain valid for their recorded images only.
- [Current demo guide](pico-client-demo-guide.docx), [readable source](pico-client-demo-guide.md). Document preparation used no hardware tests or flashes. The new convenience commands passed four software-only checks; nRF integration and measurement commands remain bench-unverified. Existing Pico feature tester is hardware-verified.

## Preserved bench status27 October2026

- **MANUAL BAUD MATRIX COMPLETE:**9600,57600,115200,230400,460800 each passed600s at full nominal baud/10 payload rate. Every requested byte returned exactly; all8radio stage counts/hashes and STM32 RX/TX counts/hashes passed; measured error increments0. B/STM32 used separately saved fixed-rate variants; A internal USB link stayed1M. Original921600 firmware/source restored and file hashes audited. [Report](2026-10-27-manual-baud-test-report.md), [suite](2026-10-27-manual-baud-matrix.json), [final audit](2026-10-27-manual-baud-final-audit.json). No automatic baud forwarding added. No test is running.
- **CURRENT:** A/B3.7.2, C/RTOS memory budget and allocation/link-event trace. STM323.6.9 unchanged. [Frozen identity](2026-10-27-budgeted-heap-images.json).
- **COMPLETE / PASSED:** continuous3600s at92,160B/s each direction,331,776,000exact bytes each way. SHA256 hashes match. Writer finished3599.995544s. All8radio stage counts and continued hashes match; all measured UART/DMA/protocol/overflow/C allocation failure increments0. The600s prefix gate passed55,296,000exact bytes each way within this same run. [Host result](2026-10-27-budgeted-heap-one-hour.json), [independent saved-evidence verification](2026-10-27-budgeted-heap-one-hour-verification.json), [radio counters](2026-10-27-budgeted-heap-one-hour.diagnostics.json). No further hardware test or flash started.
- Prior full-capacity600s passes and the3.7.1 host600s checkpoint remain preserved on their original images. Their one-hour attempts failed; do not count them as a one-hour pass or repeat completed115200/control/programming tests.
- Reallocated4KiB of measured spare RTOS heap to C allocation. Baseline confirms linked startup, zero C allocation failures and measured remaining heap. UART/network buffer sizes, window, clock/baud, DMA, wiring and Wi-Fi policy unchanged from3.7.1. A's completed local-benchmark task is disabled in production.
- Fixed host nominal9216008N1, paced92,160B/s; internal A UART/adapter1Mbaud; B calculated928882baud; STM32 calculated935064baud with unmeasured HSI tolerance. Unpaced1M USB bulk streaming is not qualified.
- Current recovery uses real socket errors and5s framed-heartbeat timeout; it does not preserve data through an established-session reset. Automatic USB baud/DTR/RTS remain deferred/incomplete.24-hour/range qualification open. STM32 debug USB absent; no internal counter claim.
- Historical results below retain image scope. Context restarts do not justify repeated completed tests.

## Preserved115200 verified result

**COMPLETE: ten minutes at115200 8N1, full offered capacity11520 payload bytes/sec in each direction, zero byte loss or corruption.** Sent/returned6,912,000 bytes; SHA256 hashes match. Sending600 seconds;602.973 seconds including final drain. All radio stage counter deltas equal6,912,000. All current radio UART/ring/network/protocol error counters and STM32 UART/DMA/queue errors are0. STM32 received/transmitted6,912,000; both FNV hashes match the generated payload.

- [Integrity result](2026-10-21-115200-dma-10min.json)
- [Counter verification and STM32 proof](2026-10-21-115200-dma-10min-verification.json)
- [Radio counters](2026-10-21-115200-dma-10min.diagnostics.json), [before baseline](2026-10-21-115200-dma-10min-before.json)
- [Exact radio/STM32 images](2026-10-21-115200-dma-images.json), [current manifest](latest-radio-build.json)

**Do not restart this completed115200 ten-minute test.** The26 October921600 run is tracked separately below. This is not a one-hour,24-hour,921600, range, latency or reconnect qualification.

## 26 October: resumed921600 test

User requested600 seconds at921600 8N1 with92160 payload bytes/sec per direction, then one hour only after a pass. This overrides the earlier fixed115200 preference for the new test. The completed115200 evidence remains scoped to its saved images.

- All three USB devices are detected. The STM32 connection check returned all256 byte values exactly at the saved115200 setting: [connection proof](2026-10-26-connected-uart-check.json). No jumper change needed.
- Both radios rebuilt/flashed as3.5.7-queued-tx-921600. A's physical J-Link VCOM configured/stored921600, handshake none. B diagnostic console and ROM preset remain115200.
- STM32 fixture flashed and verified at921600; CPU/APB clock72MHz, USART1 BRR78 (calculated923077 baud,0.16% fast). The former80MHz/BRR87 would be slower than the full nominal offered rate. The HSI clock source and DMA implementation are retained.
- Prior115200 files/images preserved in `backups/2026-10-26-before-921600/`. [New image hashes](2026-10-26-921600-images.json), [radio baseline](2026-10-26-921600-before-radio.json), [STM32 baseline](2026-10-26-921600-before-stm32.json).
- **FAILED first921600 attempt:**418304 host bytes,414827 exact downstream bytes;3477-byte deficit equals A input-ring overflow. STM32 received/returned414827 with identical hashes and zero UART/DMA/queue errors. No evidence of target-wire loss in this run. [Integrity](2026-10-26-921600-full-10min.json), [radio counters](2026-10-26-921600-full-10min.diagnostics.json), [STM32 counters](2026-10-26-921600-after-failed-stm32.json).
- Correction under validation:3.5.8 repartitions32KiB network RX +4KiB UART TX into16KiB +16KiB, saving4KiB overall. Outstanding payload window reduces16KiB to8KiB, so blocked DATA cannot strand opposite-direction credits behind more UART time than the TX queue covers. This addresses the observed overflow mechanism; validation still required.
- **FAILED buffer-layout correction:**3.5.8 at92160 B/s;169984 host bytes,61735 exact downstream bytes,108249 A ring drops. STM32 again returned every received byte with no UART/DMA/queue errors. [Result](2026-10-26-921600-balanced-full-10min.json), [images](2026-10-26-921600-balanced-images.json), [STM32](2026-10-26-balanced-after-failed-stm32.json). Buffer sizes alone did not resolve it.
-3.5.9 under validation: coalesce cumulative CREDIT and DATA frames into one full1460-byte TCP write at1440 UART bytes per batch, defer standalone credits while DATA is pending, retain standalone credits for idle/waiting senders. Added credit-wait timing counters. Working transport-delay hypothesis, not a confirmed Nagle diagnosis; this SDK exposes no TCP_NODELAY option.
- **FAILED3.5.9:**134144 host bytes,58943 exact downstream bytes,75201 A input-ring drops. Credit-wait instrumentation measured A260ms/B183ms maximum; UART and network error counters zero. Packet coalescing alone did not remove the throughput limitation. [Images](2026-10-26-coalesced-images.json), [result](2026-10-26-coalesced-full-10min.json), [counters](2026-10-26-coalesced-full-10min.diagnostics.json).
- Measured RTOS minimum free heap: A33336/B30472 bytes of51200. [Memory proof](2026-10-26-rtos-heap-evidence.json).3.6.0 uses32768 RTOS bytes, retaining about14.6KiB on A and11.8KiB on B from the previously measured minimum margins. It funds32KiB network RX +24KiB UART RX +16KiB UART TX;28KiB payload window covers311ms nominal data time, above the measured260ms credit pause. Added protocol overhead remains below the network ring size. This is a measured-latency-based change, still unqualified until tested.
- **FAILED3.6.0:**994304 host bytes,986015 downstream bytes,8289 A RX ring drops. Credit wait reduced A260ms to39ms, but both TX queues peaked16352/16384. STM32 received/returned986015 for the run with zero errors. [Images](2026-10-26-window-images.json), [result](2026-10-26-window-full-10min.json), [radio](2026-10-26-window-full-10min.diagnostics.json), [STM32](2026-10-26-window-after-failed-stm32.json).
-3.6.1 is being prepared: two DMA scratch buffers, copy/hash the next block while DMA runs, chain the prepared block directly from completion callback. Counters/hashes commit only after DMA completion. Added chain/underrun/setup-time instrumentation. This addresses the remaining worker handover cost; no pass yet. **FAILED3.6.1:**1125888 host bytes,1122271 downstream bytes,3617 A ring drops. A chained1101 DMA blocks with zero unprepared completions. Root throughput limit remains unresolved; no endurance test is running. [Images](2026-10-26-chained-images.json), [radio baseline](2026-10-26-chained-before-radio.json), [STM32 baseline](2026-10-26-chained-before-stm32.json), [result](2026-10-26-chained-full-10min.json). One-hour test remains gated on this pass.

## Previously verified115200 implementation

A/B firmware3.5.6-queued-tx-115200. Normal UART115200 8N1 on A's physical adapter link, B and the STM32. Adapter440332408 was explicitly configured/stored at115200, handshake none. Target UART remains GPIO30TX -> NucleoD2/PA10RX; GPIO29RX <- D8/PA9TX; common GND. NRST/BOOT0 wires unchanged. B diagnostic console115200.

The verified115200 radios enqueue network payload into bounded4KiB UART TX queues. Independent high-priority workers wait for DMA completion flags, avoiding blocking the network parser and polling each UART packet with a scheduler tick. Network credits acknowledge acceptance into the bounded TX queue. RX algorithms, batching and Wi-Fi power policy were retained. The STM32 fixture now uses continuous circular RX DMA plus independent queued TX DMA on DMA1 channel4/request2, instead of blocking HAL_UART_Transmit echo. Its4KiB TX queue peaked at2 bytes in the passing run.

The initial16KiB radio queue prototype reduced the C heap below the SDK's48,480-byte Wi-Fi packet pool and failed startup. The corrected4KiB queues leave53,992/53,712 bytes C heap on A/B; startup and the timed run passed. The intermediate radio-only fix removed A overflows but exposed a2048-byte skip with the old blocking STM32 fixture. These failed attempts remain saved; they are not acceptance passes.

## Client requirements

| ID | Requirement | Status / evidence |
| --- | --- | --- |
| R1 | Buffer-full / short-idle batching | **IMPLEMENTED and functionally checked on restored3.9.1:** Pico256bytes/2ms USB idle; A bounded PC batches; B1440bytes/4ms UART idle. Earlier same-image600s pass recorded27000full Pico batches; native idle tests and short echoes passed. Sustained reliability is separately failed below. |
| R2 | Wi-Fi power save disabled | **PASS for software policy:** restored-image readback A/Bstatus0/profile0. Evidence: qualification-2026-10-28/restored-policy.json. No current waveform measurement. |
| R3 | Buffering / no loss at high baud | **FAILED current rigorous confirmation:** restored3.9.1 full115200 mismatch after~95s, first mismatch offset1000543; full921600 also failed. Earlier exact-image115200600s pass and original3.7.2 one-hour921600 pass remain historical/image-scoped and do not override this reproduction failure. |
| R4 | Reconnect without reopening COM | **PASS on restored3.9.1:** B21.093s and A34.231s; same open Pico data handle,4096exact bytes after each. Six extra cycles passed on rejected3.9.2 and are separately scoped. Outage bytes not preserved. |
| R5 | Automatic baud forwarding | **PASS functional scope on exact restored3.9.1:** six nominal rates9600–921600,4096exact bytes each selection on one open USB handle. Target application must independently match. No hot-stream switching/endurance claim. |
| R6a | Wireless NRST / BOOT0 control | **PASS on restored3.9.1 Pico USB path:** actual DTR/RTS enter ROM1152008E1, GET_ID0462 and restore application exact echo. See final-features.json. |
| R6b | Actual wireless programming / verify | **INCOMPLETE for Pico path.** CubeProgrammer read-only activation failed on rejected3.9.2; no erase/write attempted; restored3.9.1 standard tool compatibility remains unqualified. Historical mailbox programming proof remains preserved. |
| R6c | Automatic USB DTR/RTS forwarding | **PASS for USB signal forwarding/ROM entry on restored3.9.1.** nRF has no manual modem buttons; Linux release helper is required after its default open for normal text. Arbitrary-terminal/tool compatibility not qualified. |

## Acceptance tests

| Test | Status |
| --- | --- |
| User-requested10min at115200, full capacity | **COMPLETE / PASSED:**6,912,000 exact bytes each way at11520 B/s. |
| 24h at115200 | **NOT RUN / NOT QUALIFIED.** Ten-minute result is not24-hour proof. |
| 24h at921600 | **NOT RUN / NOT QUALIFIED for24hours.**27October600s full-capacity pass on A/B3.6.8 plus STM323.6.9 is complete; one-hour run follows that gate. |
| Latency | **MEASURED on original images:**50 samples per size; median16.000/16.083/25.941/62.103ms for1/32/256/1024 bytes; largest observed1041.905ms. Not an absolute bound or a measurement of current images. |
| Recovery after link drop | **MEASURED on original images:** B10.828/20.616s; A25.892/25.637s. MCU-reset-triggered cases; no byte preservation during outage claimed. |
| Extra RF-only outage | Optional, not run; do not add unrequested retests. |
| One/two wall range | No verified pass. One-wall attempt failed; two walls untested. Distance/material unspecified. |

## Preserve these earlier results; do not repeat for presentation preparation

- [Original recovery/latency proof](2026-10-20-fixed-channel-reconnect-latency.json)
- [Original actual wireless programming proof](2026-10-20-current-wireless-programming.json), [log](2026-10-20-current-wireless-programming.log)
- [Failed full-capacity115200 baseline](2026-10-20-115200-full-remaining.json):1013-byte deficit matching A input-ring overflow.
- [Intermediate radio-only fix](2026-10-20-115200-queued-tx-10min-final.json): A overflow0, target return deficit2048 with old blocking fixture.
- [Original921600 failed attempt](2026-10-20-921600-one-hour-request.json)
- [Full investigation history](2026-10-21-requirements-history-before-115200-pass.md)
- [Current short demo instructions](CLIENT-DEMO-TOMORROW.md)

Backups: `backups/2026-10-20-before-115200-endurance/` preserves original921600 images/sources; `backups/2026-10-20-before-queued-tx/` preserves failed115200 baseline; `backups/2026-10-21-before-stm32-tx-dma/` preserves the old STM32 fixture. Saved115200 images retain their passing evidence. Current921600 builds and results are tracked separately above. No further tests or flashes are needed for the completed115200 milestone.

## Measured diagnosis plan

The user explicitly requested a calculated plan before further tuning. [Plan and current facts](921600-diagnosis-plan.md). Freeze3.6.1 parameters, instrument actual UART/DMA/FIFO timing, isolate local UART TX and duplex throughput, then choose a correction from measurements. No one-hour test is authorized to proceed without the requested10-minute pass.

## 26 October local UART isolation measurements

- A/B diagnostic3.6.2 retains921600, buffers/window and chained DMA scheduling. Local benchmark is explicitly triggered through a diagnostic mailbox; inactive during normal operation. No completed115200 or reset/boot/programming checks repeated.
- A local DMA TX:1,048,576 exact bytes,12.585s (83,319.5B/s), measured with host timing, RTOS ticks and DWT cycles.1024 full blocks average12,281.98us. All1023 handovers had the next buffer ready; minimum FIFO14; no fully-idle handover. [Evidence](2026-10-26-A-local-tx-cycle-measurement.json). This is below nominal92160B/s and isolates a throughput limitation before Wi-Fi/STM32.
- Local simultaneous RX/TX:1,048,576 exact bytes transmitted and received; output82,656B/s; zero RX errors. RX hash must be checked before declaring input integrity. [Evidence](2026-10-26-A-local-duplex-capacity.json). Its DWT samples are invalid because probe detach cleared trace enable; elapsed host/RTOS measurements remain valid.
- Idle DLF probe: A/B both implement mask0x3f; original values3/54 restored. [Evidence](2026-10-26-uart-fraction-width.json).
- First local benchmark used an ambiguous numeric SWD mailbox value interpreted as hex; it was stopped and is not a valid capacity result. Explicit hex values are now used.
- Next diagnostic: direct FIFO output versus DMA at the same baud to distinguish DMA bandwidth from UART bit timing. No600s921600 acceptance is currently running.

### Clock correction: confirmed local throughput improvement

A3.6.3-fast-ulp-clock-921600 uses180MHz for both UART serial clock and ULP AHB/APB (previous45MHz/40MHz), with external nominal9216008N1 unchanged. UULP APB divider10 keeps it18MHz. Local DMA TX improved83,319.5 to92,174.4B/s:1,048,576 exact bytes in11.376s;1024 full DMA blocks average11,101.50us; no idle or unprepared handovers. [Evidence](2026-10-26-A-fast-clock-capacity.json). This confirms the clock-domain throughput bottleneck locally; it is not yet full-path endurance proof.

- Corrected-clock local duplex returned all1,048,576 bytes exactly and its RX FNV2630655429 matches the generated stream. Output91,843.4B/s is still below92,160B/s, so do not treat it as a full-capacity pass. [Evidence](2026-10-26-A-fast-clock-duplex.json). No unprepared/idle block handovers; average full-block duration11,141.75us versus11,101.50us TX-only.
-3.6.4 under validation budgets0.75% UART clock headroom, about twice the observed0.36% duplex cost, instead of allocating more buffers. Terminal remains nominal9216008N1; actual generated radio baud is explicitly929032A(+0.806%) /928882B(+0.790%), STM32 remains923077. This is not exact921600 bit timing and must be reported honestly. ROM preset remains1152008E1. Local duplex capacity must pass before the600s full-path test.

-3.6.4 local duplex capacity confirmed92,532.3B/s:1,048,576 exact TX bytes, RX count/hash exact (FNV2630655429), no errors/unprepared/idle handovers. [Evidence](2026-10-26-A-margin-duplex.json). [Frozen radio/source/STM32 identities and actual rates](2026-10-26-clock-margin-images.json). Full-path acceptance pending; no one-hour/24-hour pass claimed.

###3.6.4 full-path test in progress
Nominal921600,92,160B/s offered,600s requested,55,296,000 bytes per direction. [Result path](2026-10-26-clock-margin-full-10min.json). A/B linked, normal diagnostic mailbox inactive, baseline radio counters recorded. STM32 ST-LINK USB interface is no longer enumerated (two radios remain visible); target UART echo is being verified end to end. STM32 internal counters are unavailable until the probe enumerates again; do not claim its current error counters were read. [Baseline STM32 failure](2026-10-26-clock-margin-before-stm32.json).

###3.6.4 full-path result: A overflow resolved, target leg still failed

- **FAILED:**795,648 host/A input/B TX bytes,795,639 B RX/A output bytes. Nine missing bytes; first mismatch783,632=3061*256+16. All A/B network/ring overflows0; A credit wait0. [Integrity](2026-10-26-clock-margin-full-10min.json), [counters](2026-10-26-clock-margin-full-10min.diagnostics.json). This resolves the prior A throughput bottleneck but does not qualify end-to-end endurance.
- The9-byte skip begins16 bytes after a256-byte RX boundary, consistent with B's16-byte FIFO overflowing during single-descriptor DMA rearm. STM32 internal counters unavailable because its ST-LINK USB remains absent; this cause is inferred from the alignment/stage counts until correction is validated.
- B3.6.5-target-pingpong-921600 under validation: continuously armed primary/alternate256-byte RX descriptors; hardware switches buffers without waiting for the completion ISR. SDK IRQ hook services only RX channel24; TX/other channels retain SDK handling. Idle partial publication and ROM mode initialization use the same receiver. Added RX block/stall/line-error counters. A remains3.6.4. No completed reset/boot/programming demonstration repeated.

###3.6.5 result and receive-publication correction
- **FAILED throughput:**2,896,384 host bytes;2,894,943 downstream bytes;1,441 A ring drops. B received exactly every byte it sent, with11,308 completed ping-pong blocks and zero RX stalls/line errors. The nine-byte target-leg gap did not recur in this32s run. [Result](2026-10-26-pingpong-full-10min.json), [counters](2026-10-26-pingpong-full-10min.diagnostics.json). No endurance pass.
- B's TX FIFO had become fully idle at2,567/2,961 chained completions. Its full-block average was below required throughput while RX processing ran before TX callback handling.
- B3.6.6-batched-target-rx-921600 replaces256 separate per-byte ring callbacks/barriers with one bounded bulk copy. The original SDK IRQ dispatcher is restored byte-for-byte; only RX channel24's callback is replaced after initial driver setup. No extra queue memory, baud, window or buffer changes. Hardware primary/alternate RX remains active, and ROM mode uses the same receiver initialization. Validation pending.

###3.6.6 extended diagnostic: exact data, source below requested capacity
- Run stopped deliberately at255.136s after22,788,523 exactly matching returned bytes (no mismatch). All22,795,776 accepted host bytes reached B and were returned at the radio counters; the host reader stopped before the final drain. A/B ring/network/line errors and B RX DMA stalls0. [Interrupted result](2026-10-26-batched-rx-full-10min.json), [counters](2026-10-26-batched-rx-full-10min.diagnostics.json). This is not a completed600s pass.
- Actual source acceptance averaged~89,348B/s despite the92160B/s request; the onboard adapter/internal UART path backpressured host writes. Do not call this full-capacity921600 evidence.
- A3.6.7 uses a separate internal USB-adapter UART at1,000,000baud (Commander VCOM stored to match), with the terminal nominal setting921600 and B/STM32 target baud unchanged from3.6.4/3.6.6. A RX now also uses hardware primary/alternate descriptors to remove rearm gaps on the faster internal path. This explicit fixed internal link is not automatic USB baud forwarding. Full-path600s must now deliver55,296,000 exact bytes at the requested92160B/s offered load.

### Corrected full-capacity fixture now running
A3.6.7/B3.6.6, internal adapter1Mbaud, terminal nominal921600, target baud unchanged.20/40/60/80s host progress confirms92,160B/s (about1,843,200bytes per20s), with no mismatch so far. [Live result path](2026-10-26-1m-host-link-full-10min.json), [baseline](2026-10-26-1m-host-link-before-radio.json), [images and clock rates](2026-10-26-1m-host-link-images.json). No completion or one-hour claim until final validation.

### Full-capacity fixture result: FAILED at309seconds
- A3.6.7/B3.6.6 sustained the requested92160B/s source, with28,352,160 exact returned bytes before a mismatch at309.044s. Host sent28,481,536bytes. A restarted with retained reason2; A input-ring drops58,977 and B input-ring drops121,825. UART line/TX error counters0; STM32 debug counters unavailable. No endurance pass.
- [Final result](2026-10-26-1m-host-link-full-10min.json), [post-test counters](2026-10-26-1m-host-link-full-10min.diagnostics.json), [unchanged image identity](2026-10-26-1m-host-link-images.json). B's post-test receive total exceeds A's retained send total; the retained failure snapshot and transport accounting need diagnosis before attributing this to wiring or noise.
- **27 October: no test running.** Investigating transport/queue failure handling; no completed115200/reset/boot/programming check is being repeated. One-hour test remains gated on the600s pass.

###3.6.8 transport guard under validation
- Removed both outer whole-frame ENOBUFS retries. The inner send loop retains partial-write position and reserves credit once; a terminal failure ends that session. The SDK copies the socket buffer before returning success (local4.0.2 `sli_si91x_driver_send_socket_data`), so buffer lifetime does not require a change.
- Added monotonic16-bit DATA sequence checks and first-stop reason/detail, plus retained credit/sequence/pending-byte snapshots. UART queue timeouts now increment an error counter. No baud, buffers, window, clocks or wiring changed. This distinguishes duplicate/out-of-order frames, explicit remote/AP closure and a local UART timeout.
- Both images build. A new921600 run is necessary because the prior run failed and transport failure handling changed. Completed115200 and historical control/recovery tests remain untouched.

-3.6.8 A/B flashed successfully; both linked with zero DATA counters and no current stop reason. [Baseline](2026-10-27-transport-guard-before-radio.json), [image/source identity](2026-10-27-transport-guard-images.json).600s/92160B/s each direction starting; not a pass until final exact-byte and counter verification.

###3.6.8 result: target-side2048-byte gap
- **FAILED at296.49s:**27,313,664host/A/B TX bytes,27,311,616B RX/host return bytes. Exactly2048bytes absent between B's target UART TX and RX. Radio input/network overflows, UART line errors, RX DMA stalls, DATA sequence errors, credit waits and queue timeouts all0. No radio reset during transfer. [Result](2026-10-27-transport-guard-full-10min.json), [counters](2026-10-27-transport-guard-full-10min.diagnostics.json).
- Transport counters/hashes agree in both directions; the missing count equals the STM32 circular RX buffer length. This localizes the remaining failure to the target UART/fixture leg, not a demonstrated TCP loss. Inspect STM32 producer accounting and clock margin before the next run. No test currently running.

### STM32 fixture3.6.9 correction prepared
- The source used a2048-byte modulo-only RX index and4096-byte TX queue. Once backpressure allows DMA to lap the reader, an entire2048-byte lap disappears from its count. Added transfer-complete lap accounting and explicit overrun-byte detection; RX/TX buffers are now16KiB each (35,220bytes total BSS, within160KiB RAM).
- HSI-derived72MHz/78 gives only0.16% nominal TX capacity margin. The [STM32L452 datasheet](https://www.st.com/resource/en/datasheet/stm32l452re.pdf) specifies HSI initial tolerance greater than that margin, with further temperature/supply drift. Exact oscillator error is not measured because ST-LINK remains absent. Fixture BRR77 gives calculated935064baud (+1.46% nominal), only0.665% above B's928882baud. This is explicit test-fixture clock headroom, not exact921600 bit timing or automatic baud forwarding. Ambient qualification only; no temperature-range guarantee.
- Built successfully. Need to install this correction through the existing wireless ROM programming path because STM32 USB debug is unavailable. This programming is necessary for the new fixture change; it is not a repeat demonstration of the completed boot/programming requirements. Original fixture/source preserved in `backups/2026-10-27-before-stm32-rate-margin/`.

- STM32 fixture3.6.9 installed and byte-verified successfully over UART/Wi-Fi, with no SWD programming fallback. Application exact echo restored. [Installation](2026-10-27-stm32-fixture-margin-install.json), [programmer log](2026-10-27-stm32-fixture-margin-install.log), [new identities and actual baud](2026-10-27-stm32-margin-images.json). This newly verifies the necessary installation/restore operation on A/B3.6.8; prior full programming/recovery evidence remains preserved. Next run targets only the failed921600 endurance item.

- New600s at92160B/s each direction starting after verified STM32 installation. No radio reflash/reset; [per-stage baseline](2026-10-27-stm32-margin-before-radio.json). Hash verification continues each stage from its own baseline, so previous ROM command/response traffic does not require resetting boards. [Result](2026-10-27-stm32-margin-full-10min.json).

##27 October921600 full-capacity10minute milestone COMPLETE
- **PASSED:** A/B3.6.8 transport guards and STM32 fixture3.6.9.600seconds at92,160payload bytes/sec each direction;55,296,000exact bytes sent and returned. Writer completed599.99565s. All8radio stage byte deltas equal55,296,000; all8per-stage FNV continuations match the expected stream; radio line/protocol/sequence/DMA-stall/queue/overflow error deltas0. STM32 internal counters remain unavailable; end-to-end integrity passed.
- [Host result](2026-10-27-stm32-margin-full-10min.json), [verification](2026-10-27-stm32-margin-full-10min-verification.json), [radio counters](2026-10-27-stm32-margin-full-10min.diagnostics.json), [baseline](2026-10-27-stm32-margin-before-radio.json), [image/source identity and clock offsets](2026-10-27-stm32-margin-images.json).
- Nominal capacity qualification uses A's fixed internal1Mbaud link, B calculated928882baud, STM32 calculated935064baud. No automatic USB baud forwarding or exact921600bit timing claimed.
- **Do not repeat this COMPLETE check without a relevant change or explicit request.** The previously requested one-hour run is now authorized by the passed gate, using the same images.24-hour operation remains unqualified.

### One-hour921600 validation now running
Requested3600s at92,160B/s each direction,331,776,000bytes each way. Same images, wiring and rates as the completed600s pass; no reflash or reset. [Baseline](2026-10-27-921600-one-hour-before-radio.json), [live result](2026-10-27-921600-one-hour.json). No one-hour/24-hour pass claimed until final verification.

### High-speed source scope
The completed high-speed test and current one-hour run pace input at the full nominal9216008N1 payload capacity,92,160B/s. A's fixed internal UART is1Mbaud to provide measured adapter capacity/headroom. J-Link ignores the terminal line setting; an unpaced USB bulk writer may therefore exceed the target's capacity. Unpaced bulk operation at the internal1Mrate is not qualified and needs source pacing or a host interface with integrated flow control. This does not change the recorded exact-byte full-nominal-rate pass; it limits the general invisible-cable claim until USB interface integration.

### One-hour result FAILED at2317.8seconds
-213,236,160 exact returned bytes before A restart injected00; host sent213,606,400. B retained stop7 (DATA sequence discontinuity); A stop12 (AP disconnect) followed. B UART/ring/network errors0; A ring drops followed the transport stop. The600s pass remains valid for its saved images. No one-hour pass.
- [Result](2026-10-27-921600-one-hour.json), [diagnostics](2026-10-27-921600-one-hour.diagnostics.json). Diagnose DATA sequence detail before another run; no test currently running.

###3.7.0 duplicate-safe transport correction
- B expected DATA sequence9279 and received9278, the immediately preceding accepted sequence. Radio DMA/line errors were0 before teardown. A's AP-disconnect stop happened after B's sequence-triggered restart; it was secondary. The lower transport's reason for replay remains unconfirmed.
- Receiver now suppresses only a repeat of the immediately previous sequence with identical length and every payload byte equal. It reuses the caller's unchanged previous RX buffer, adding no payload storage. A changed payload under the same sequence is fatal (stop13); skipped/reordered sequences remain fatal (stop7). Neither bytes nor delivery credits advance on a suppressed replay. Stale cumulative credits within the bounded window are ignored; credits beyond sent bytes still fail.
- Exact duplicate, changed duplicate, length mismatch, gap and sequence/counter wrap checks passed in software; send partial-write/ENOBUFS checks still pass. Baud, clocks, DMA and buffers remain unchanged. Both new images build. The original600s pass remains valid only for A/B3.6.8. New3.7.0 validation is necessary for this specific transport correction; no completed115200 or control/recovery demonstration is repeated.

-3.7.0 A/B flashed and linked. [Frozen identities](2026-10-27-duplicate-safe-images.json), [baseline](2026-10-27-duplicate-safe-before-radio.json). New600s transport validation starting at92,160B/s; STM32 unchanged. [Result](2026-10-27-duplicate-safe-full-10min.json).

##3.7.0 full-capacity600second validation COMPLETE
**PASSED:**55,296,000exact bytes per direction at92,160B/s. All8 stage byte deltas and expected hash continuations match. All radio error/overflow/queue-timeout/sequence-error deltas0. No duplicate replay observed during these600s; fault-injection software tests verify suppression behavior. [Host result](2026-10-27-duplicate-safe-full-10min.json), [verification](2026-10-27-duplicate-safe-full-10min-verification.json), [image identity](2026-10-27-duplicate-safe-images.json). Do not repeat this completed test without a relevant change or explicit request.

The requested one-hour validation now runs on these same images. Its baseline reuses the drained, verified600s post-test capture, avoiding another probe read. [Baseline](2026-10-27-duplicate-safe-one-hour-before-radio.json), [live result](2026-10-27-duplicate-safe-one-hour.json). No one-hour pass yet.

###3.7.0 one-hour result FAILED on AP notification
B suppressed7byte-identical previous DATA repeats; all accepted B payload/UART/return counts and hashes match. A retained stop12 (AP client-disconnect notification), with no prior protocol/UART/network errors; A ring overflow followed the stalled session. [Result](2026-10-27-duplicate-safe-one-hour.json), [diagnostics](2026-10-27-duplicate-safe-one-hour.diagnostics.json). No test currently running.

Current default AP configuration uses NULL-based keepalive,100ms beacons and idle period255. SDK documentation says its NULL probe can deauthenticate a station after retry failure. Existing AP callback also immediately resets the whole bridge for any client disconnect event.3.7.1 will disable redundant AP keepalive and treat generic Wi-Fi client events as advisory; real socket failures and existing5s framed-heartbeat timeout remain authoritative. Record event MACs/ticks to distinguish peer events. [Vendor keepalive definitions](https://docs.silabs.com/wiseconnect/4.0.2/wiseconnect-api-reference-guide-wi-fi/sl-wifi-constants). No heap-size change: current C-heap capacity~50,480bytes versus48,480-byte pool is tight but allocator failure has not been established.

###3.7.1 continuous qualification starting
Both radios flashed successfully and linked; AP applied keepalive0. [Baseline](2026-10-27-ap-heartbeat-before-radio.json), [frozen identities](2026-10-27-ap-heartbeat-images.json). New host runner records the exact600s prefix SHA256/count/rate gate, then continues the same open COM session to3600s total. Final radio stage hashes/counters qualify the entire duration after the run stops; no probe reads during traffic.

###3.7.1 exact600s prefix checkpoint PASSED
The continuous test verified55,296,000exact prefix bytes each way at92,160B/s. [Checkpoint](2026-10-27-ap-heartbeat-one-hour.checkpoint.json). It continues without reopening COM or altering firmware toward3600s total; no stop/restart or repeat of the checkpoint. Radio counters will be read and qualified after final transfer, not mid-run.

###3.7.1 result FAILED on actual peer disassociation
The600s exact host prefix passed. The full run failed after~31minutes:170,979,488exact returned bytes, then5s without data and A heartbeat reset. AP disconnect event MAC equals the connected B MAC; disabling NULL keepalive/advisory handling did not prevent the actual disassociation. B suppressed12exact repeats and returned every accepted payload byte; its line/DMA/ring/sequence errors0. No claim of a one-hour pass. [Result](2026-10-27-ap-heartbeat-one-hour.json), [diagnostics](2026-10-27-ap-heartbeat-one-hour.diagnostics.json). No test running.

Passive memory capture shows C-heap capacities50,400A/50,432B, committed49,504A/50,128B, leaving896A/304B outside the malloc arena. RTOS minimum free13,232A/12,040B of32KiB remains stable. Allocation failure has not yet been instrumented; do not attribute peer disassociation to memory without additional evidence. Static IP is already configured; DHCP is not the cause.

###3.7.2 measured C allocation budget
C heap is a separate pool from the RTOS heap. Reduce RTOS reservation32KiB→28KiB, retaining measured worst-case margin at least7,944bytes on B (12,040−4,096) and over9KiB on A. Stop creating A's completed local-benchmark task; its1KiB pattern/static data is no longer linked. Keep UART/network buffers, window, baud and DMA unchanged. Add transparent malloc/calloc/realloc failure counters and retained station event/status/data. Allocation wrapper fault-injection checks pass; AP-event and exact-replay receive checks still pass. No allocator failure is yet proven in the earlier failed image.

###3.7.2 qualification starting
Both boards linked with zero allocation failures. [Startup/memory baseline](2026-10-27-budgeted-heap-before-radio.json). Continuous3600s with600s exact-byte/full-rate prefix gate starting; counters read only after transfer. No new programming/reset/reconnect demonstration.

###3.7.2 ten-minute host prefix PASSED
[Checkpoint](2026-10-27-budgeted-heap-one-hour.checkpoint.json):55,296,000exact bytes each direction at92,160B/s; source reached the prefix in~600s, received SHA256 matches. Continuing the same open COM session to3600s total. No stop/start or hardware probe during traffic. Final radio/C allocation counters remain pending.

##27 October921600 one-hour milestone COMPLETE

- A/B3.7.2-budgeted-heap-921600 with STM32 fixture3.6.9 passed3600s at92,160B/s each direction.331,776,000exact bytes each way; matching SHA256f33801d0b65e3c3f5ab3728d1f619a9dde7f6989d2ef4c942f90d987c63bf47b. All8stage count/hash checks passed; measured radio error and C allocation failure increments0. Duplicate suppression counts0 in this run.
- The600s55,296,000byte prefix is part of this same continuous run. No outage was injected. STM32 internal debug counters remain unavailable.
- [Verification](2026-10-27-budgeted-heap-one-hour-verification.json), [host](2026-10-27-budgeted-heap-one-hour.json), [identities](2026-10-27-budgeted-heap-images.json). No current test running. Do not repeat this completed milestone without a relevant change or explicit request.
- Scope remains paced full nominal9216008N1 payload capacity, with the documented internal1Mbaud adapter and calculated target clock offsets. Unpaced USB,24hours, current-image reconnect/latency and wall range are not qualified by this run.

## Manual fixed-baud matrix27 October2026

User requested manually configured baud rates, with600s each. These are fixed-rate tests, not automatic baud matching.
Suite status: **passed**. Original921600 restored: **True**.

| Nominal baud8N1 | Seconds | Payload B/s each direction | Exact bytes each direction when passed | Result |
| --- | --- | --- | --- | --- |
| 9,600 | 600 | 960 | 576,000 | [PASSED](2026-10-27-manual-9600-10min-verification.json) |
| 57,600 | 600 | 5,760 | 3,456,000 | [PASSED](2026-10-27-manual-57600-10min-verification.json) |
| 115,200 | 600 | 11,520 | 6,912,000 | [PASSED](2026-10-27-manual-115200-10min-verification.json) |
| 230,400 | 600 | 23,040 | 13,824,000 | [PASSED](2026-10-27-manual-230400-10min-verification.json) |
| 460,800 | 600 | 46,080 | 27,648,000 | [PASSED](2026-10-27-manual-460800-10min-verification.json) |

Each pass includes every returned byte, SHA256, all8radio stage byte counts/FNV hashes, and STM32 RX/TX count/hash checks. Measured radio and STM32 error increments must be0.

A retains its separate fixed1Mbaud internal USB link. B and STM32 are rebuilt/flashed for each selected nominal rate. As in the preserved high-speed test, B uses0.75% requested clock headroom and the STM32 fixture about1.46%; exact calculated rates and frozen images are in each image manifest. HSI tolerance is unmeasured. These tests do not prove automatic forwarding, exact nominal bit timing,24hours or temperature-range operation.

The completed921600 one-hour pass remains preserved:331,776,000exact bytes each way at92,160B/s. It is not repeated in this matrix. Passing115200 results on prior images remain separately preserved; this new run qualifies the new fixed-rate variant.

[Suite record](2026-10-27-manual-baud-matrix.json). Source and images were backed up before changes. No reset/boot/programming acceptance demonstration repeated; flashing here is necessary to select the requested test variants.


##28 October — separate PC-helper baud prototype prepared

The user explicitly authorized development of A/B baud-selection firmware, a simple Linux GUI and a matching STM32 fixture while the hardware is unavailable. This supersedes the earlier research-only/deferred note **for this temporary helper-app experiment**. Automatic forwarding from an ordinary terminal and automatic USB DTR/RTS remain incomplete.

- **DEVELOPMENT COMPLETE / HARDWARE PENDING:** isolated3.8.0 prototype in [experiments/pc-baud](../experiments/pc-baud/README.md). A/B radio protocol3; explicit baud request/query/acknowledgement; CRC-protected framed PC link; idle-only B UART changes; STM32 fixture rate control through its ST-LINK serial port. Six nominal8N1 rates:9600,57600,115200,230400,460800,921600.
- A/B RPS and STM32 ELF builds passed.13 software tests passed, including actual firmware handlers with platform stubs and the real pySerial client over Linux pseudo-terminals. Those are software checks, not board measurements. GUI visual check remains pending because the development session cannot access a graphical display.
- [Build/image identities](../experiments/pc-baud/build-manifest.json), [software log](../experiments/pc-baud/software-tests.log), [instructions and evening checks](../experiments/pc-baud/README.md).
- Original3.7.2/3.6.9 source and image hashes audited unchanged against the experiment baseline manifest. No hardware device opened, no flash, and no hardware test performed during this development turn. Current board images therefore remain as previously recorded.
- Preserve all completed acceptance evidence above. The old endurance passes do not qualify3.8.0. Later testing should start with only the new-feature sequence: apply115200 and921600, short exact binary echo, then six-rate functional switching. Do not restart completed reset/boot/programming or endurance checks merely as preparation.
- The app keeps A's adapter link fixed1M. B retains0.75% requested throughput headroom; the fixture retains approximately1.46%. Changes require quiet traffic. Short4096-byte stop-and-wait echo does not qualify full-capacity streaming or24hours. Runtime settings are not persistent across power loss.


##28 October — Pico USB integration in progress

User has received a PicoRP2040, confirmed the GP0TX→A GPIO29RX, GP1RX←A GPIO30TX and GND wiring, and connected all boards. New development is isolated in `experiments/pico-bridge/`. A/B3.9.0 use radio protocol4; Pico uses framed link2 on fixed1M UART. All images build; native firmware/engine software checks passed. [Image identities](../experiments/pico-bridge/build-manifest.json). Original source/images audited unchanged.

The following installation and checks are necessary for the newly requested USB interface, not a repetition of the completed old-image acceptance sequence: load newA/B/Pico and the prepared adjustable STM32 fixture; verify USB line-coding and exact binary echo at selected rates; then verify USB DTR/RTS→NRST/BOOT0 through this new path. Hardware results pending. Prior24h/endurance/range status remains unchanged.

##28 October — Pico integration checkpoint: USB connection unavailable

- Current experimental images are built and flashed: B hardware-TEMT check correction, Pico UART-error recovery, and A shared ACK-sequence visibility/guard diagnostics. Identity: [manifest](../experiments/pico-bridge/build-manifest.json). Original acceptance images/results remain preserved.
- Before the latest A correction, short4096-byte exact echoes passed at115200,921600 and9600 on one open COM handle. The following57600 check stopped on an A guard. These partial results qualify only the archived image set: [result](../experiments/pico-bridge/before-ACK-guard-fix-hardware-result.json), [identities](../experiments/pico-bridge/before-ACK-guard-fix-images.json). The exact guard trigger was not proven; the new A correction still needs validation.
- Latest attempt did not complete a payload check: [result](../experiments/pico-bridge/hardware-result.json). Linux logged Pico cafe:4012 serial E66488C15F39C034 enumerating and immediately disconnecting at21:41:28,21:41:57,21:42:07 and21:43:11 local time. At the latest inventory, A, B and Nucleo were present; Pico and its two COM ports were absent. Cable/contact/power is a suspected cause, not a proven diagnosis.
- Next physical action: replace the Pico Micro-USB data cable and connect directly to the computer; leave jumper wiring and current firmware unchanged. No hardware test is running. Do not repeat original completed endurance checks.
- Automatic USB baud forwarding and USB DTR/RTS→reset/ROM verification remain INCOMPLETE on this new path. Prior24-hour/range/endurance qualification scope remains unchanged.

### Pico USB connection restored — latest check28 October
Pico cafe:4012 is present with data ttyACM3 and status ttyACM4, with no Pico disconnect in the inspected recent kernel log. Three consecutive diagnostic queries returned ready=1, synced=1, baud=9600, parity=N, dtr=0, rts=0, errors=0. This is a status/connectivity check only, not payload or reset/ROM qualification. A and B are present. Nucleo ST-LINK USB is now absent (kernel disconnect21:54:04 local); reconnect its mini-USB data cable to resume the pending new-feature checks. No flash or payload test was performed in this check.

## 28 October — Pico USB baud and DTR/RTS functional milestone COMPLETE

All four boards were present; the previously interrupted new-feature sequence completed on the current A/B/Pico images without further flashing. [Frozen functional result with image SHA256 identities](../experiments/pico-bridge/2026-10-28-pico-functional-pass.json), [current manifest](../experiments/pico-bridge/build-manifest.json), [wiring/demo instructions](../experiments/pico-bridge/README.md).

- **COMPLETE, functional baud forwarding:** ordinary USB CDC line-coding changes forwarded through Pico→A→B at 115200→921600→9600→57600→230400→460800→115200. Each setting passed 4,096 exact binary bytes in each direction with matching SHA256; the same data COM handle stayed open. STM32 fixture settings were independently adjusted over ST-LINK. This does not make an arbitrary target application change its own baud automatically.
- **COMPLETE, functional USB DTR/RTS forwarding:** USB control signals drove the target reset/BOOT0 sequence. At 115200 8E1, STM32 ROM synchronization returned ACK79 and read-only GET_ID returned0462. This validates the new USB control path beyond mailbox acknowledgements. DTR asserted holds NRST low; RTS asserted holds BOOT0 high. Compatibility with arbitrary flashing utilities and actual flash writes through this new path remain unqualified.
- **COMPLETE, application restoration:** reset with BOOT0 released restored the application at1152008N1, followed by256 exact bytes each direction.
- Startup/reset transition bytes were recorded separately before payload checks. Changes were made with traffic stopped. Image files were audited against every SHA256 in the passing report; all matched.
- Test ran16:57:31–16:57:45UTC (21:57:31–21:57:45 Asia/Karachi). No test is running. Do not repeat these COMPLETE new-feature checks without a relevant change or explicit live demonstration request. No old endurance/programming acceptance sequence was repeated.
- Prior one-hour921600 and manual-baud endurance evidence remains scoped to the preserved original images. New Pico-path sustained load,24hours, latency/reconnect and wall-range qualification are not established by this short functional pass.

##28 October demo documentation prepared without hardware activity

Created the seven-page [Pico demonstration guide](pico-client-demo-guide.docx), [Markdown source](pico-client-demo-guide.md), wiring/data-path diagram and `docs/demo-support.py`. All seven rendered document pages were visually inspected. Four software-only checks passed for modem flag release, stream boundary independence, exact echo acceptance and corruption rejection. No serial device was opened, no radio reset and no flash was performed during document preparation. Future commands in the guide are instructions, not measured passes. New helper commands remain bench-unverified; use the saved hardware-verified Pico feature result for completed functionality. Current R1 scope clarification records that B keeps idle batching, while Pico-to-A USB chunks are forwarded immediately without a newly qualified idle coalescing rule.

##28 October explicit rigorous rerun in progress

User explicitly requested testing everything again before the demonstration and a client report. This authorizes the bounded reruns below; no24-hour test has been claimed or started. Evidence is under `experiments/pico-bridge/qualification-2026-10-28/`.

- Initial image SHA256 audit and native software regressions passed. Actual serial-open/release/fixture commands representative of nRF defaults passed three text echoes at115200→921600→115200. Actual GUI clicks have not been driven.
- All seven baud selections passed exact4096-byte echoes again. The first ROM attempt then saw a late reset-transition00 before its ACK. The host tester now bounds and records only the pre-ACK ROM synchronization prefix, rejects NACK/oversized prefixes, and keeps normal payload/GET_ID checks exact. Three ROM GET_ID0462 / application-return cycles passed afterward.
- Wi-Fi power policy readback passed for A and B: status0/profile0. No new power waveform or latency claim.
- Sustained115200 failed after81920 correct returned bytes; not a600s pass. Follow-up logged reproductions stopped as well. A retained a CRC/parser failure followed by DATA sequence gap, with DMA-stall and UART-line-error counters0; STM32 reported noUART/DMA/queue errors. This is a new Pico transport failure, not a pass inherited from original images.
- Narrow A diagnostic images retained raw failed frame and first-stop details. Original A images and identities archived before each change. Investigation found A's newly used USART0 inherited ULP-reference fractional clock configuration (readback26.666MHz) whereas B used the interface PLL integer divider. Current candidate A3.9.1 selects the same160MHz PLL integer clock to generate exact1Mbaud. Target rates, buffers and protocol remain unchanged. This is a candidate correction under validation; B, Pico and STM32 images are unchanged.
- Current short/long tests must be tied to the updated manifest. The previous3.9.0 functional pass remains archived and does not qualify the changed A image. No client readiness claim until the failing sustained case passes.

### Final transport candidate prepared
A3.9.1 now uses stable160MHz INTFPLL with an integer divider. A clock-only30s115200 run passed345600exact bytes each way with no new Pico fault; its subsequent incomplete run was stopped deliberately for the required batching/bandwidth correction and is not a600s pass. Pico3.9.1 now accumulates up to256bytes or2ms of actual USB idle, with bounded pending data drained before coding changes; native tests cover short-chunk coalescing and the change-during-partial-batch case. Physical Pico/A UART is now2M8N1: full921600 payload plus minimum DATA/ACK framing requires100800B/s before escaping, greater than the old1M link's100000B/s capacity. Both2M dividers are calculated exact. B/STM32 images, selected target rates, buffers and radio protocol are unchanged. Final2M baud/ROM/application feature sequence passed; final600s115200 transfer is now running under the updated manifest. Only this final image set will qualify the sustained results.

## Final2M Pico115200600s milestone COMPLETE
Current A/Pico3.9.1-PLL-batching-2M with unchanged B3.9.0 and STM32 fixture3.8.0 passed600s at11520B/s each direction:6912000exact bytes each way, matching SHA256. All8stage radio/STM32 byte deltas and FNV continuations passed; every measured radio/STM32 error increment and Pico session-error increment0. [Host/counters](../experiments/pico-bridge/qualification-2026-10-28/final-2M-115200-full-600s.json), [verification](../experiments/pico-bridge/qualification-2026-10-28/final-2M-115200-full-600s-verification.json). This result belongs to the exact2M images in its manifest. The earlier1M link attempts stopped before completion and are not600s passes. Final2M921600600s run is now in progress, not yet passed. The user expressly requested this complete rigorous rerun; do not repeat the completed final115200 milestone without a relevant change or another explicit request.

###921600 final2M candidate failed on application batching; A3.9.2 correction
The3.9.1 final2M115200600s pass remains preserved for that image. Its921600 attempt failed: source backpressure reduced accepted rate far below92160B/s and B's return UART ring overflowed3553bytes. A/B/STM32 physical UART error increments0; STM32 received and returned662528bytes with matching stage hashes. A still sent one TCP DATA frame per256-byte Pico block, unlike the original full-MSS transport. [Failure/counters](../experiments/pico-bridge/qualification-2026-10-28/final-2M-921600-full-600s.json).

A3.9.2 now packs received PC blocks into1440-byte socket writes, retaining exact partial-block offsets and acknowledging only fully forwarded PC packets. Short tails flush after idle; one socket write per service call lets incoming ACK/DATA be parsed first. Native tests exercise2048bytes spanning1440+608, the160-byte partial-block boundary and cumulative ACK IDs. A's physical UART RX ring24KiB→16KiB funds the1440-byte batch and increases C-heap reserve by about6.6KiB; the8×256-byte PC window bounds accepted raw UART payload to2KiB plus framing/control. Pico/B/STM32 images and rates are unchanged. The failed921600600s case is now running on3.9.2. No pass claimed yet; the changed A also needs its own115200 qualification.

## Rigorous rerun checkpoint and rejected variants

A3.9.2 MSS packing did not fix the high-rate problem. Its600s request failed at306s with a first mismatch at byte2987487; B return-ring overflow was observed. Adding pending-queue-aware standalone-credit scheduling also failed the921600 offered-load criterion and stalled a subsequent115200 source with a host write timeout. No radio/STM32 line errors increased in that short115200 failure. The NWP-small-packet explanation remains a hypothesis, not an established root cause. Pico diagnostic counters showed its input waiting for A acknowledgements while USB data was available; UART overrun counter0.

Both changed A images and the diagnostic Pico FLOW image are rejected for the demonstration. Source and images are preserved under qualification-2026-10-28/rejected-MSS-credit. Exact A3.9.1/Pico3.9.1 images from the completed2M115200600s pass have been restored, with every image SHA256 checked against that pass. A fresh600s115200 confirmation is running as restored-115200-600s.json. This specific rerun follows a relevant firmware restoration after a demonstrated regression.

The rejected MSS bundle passed six-rate functional checks and six same-open-COM reset recovery checks (14.321–21.854s); those results are image-scoped and are not silently transferred to the restored A. Standard STM32CubeProgrammer read-only activation failed; no erase/write was attempted, and the application was restored with exact echo. Current Pico USB ROM-ID control has passed, but flash-tool compatibility remains incomplete.

## Rigorous rerun final measured status

The exact restored3.9.1 bundle also failed its fresh full115200 confirmation after95s:1078784accepted host bytes,1000704received, first mismatch1000543. This disproves a reliable current sustained115200 claim despite its earlier600s pass on the same hashes. A's counter capture in this restoration run accidentally referenced the stale3.9.2 build ELF; those A counters are excluded. The host mismatch remains valid. The helper now uses archived image ELFs, checks manifest hashes before reads and parses memory lines without matching across newlines. A rebuilt from restored sources matches its archived image exactly. B RSSI readback was-43dBm/status0; noise/range has not been established as root cause.

Restored-image functional and housekeeping results: all six-rate feature checks remain backed by the exact matching final-2M-functional.json; default-open/Linux-release/fixture/text echo passed115200→921600→115200 (restored-nrf-semantics.json). Policy A/B0/0 passed. Two necessary restore-specific recovery cycles passed B21.093s/A34.231s with one COM and4096exact bytes each. Latency passed400exact echoes:50samples each1/32/256/1024bytes at115200 and921600. One-byte median19.091/17.009ms respectively; largest observed961.115ms. No absolute latency bound is claimed.

No hardware test remains running. Final boards are restored to the exact3.9.1 bundle and normal115200 fixture/data setting. High-rate/sustained issues and standard flashing compatibility are open;24hours and wall-range remain unqualified. The presentation guide restricts the live demonstration to verified short functional behaviour. See pico-client-testing-report.docx and qualification-summary.json for evidence and limits. Do not restart completed tests just to regenerate these documents.

Deliverables finalized: four-page testing report and seven-page demonstration guide rendered and visually checked on every page. qualification-summary.json records their hashes and the final image audit. No test is running; final readback ready1,1152008N1,DTR0,RTS0.

##29 October focused blocker resolution in progress

User explicitly requested resolving the sustained-data and programmer blockers for delivery. Completed feature checks are not being restarted as setup.29October evidence is isolated under experiments/pico-bridge/qualification-2026-10-29, with exact3.9.1 baseline backups.

A diagnostic local PC/Pico/UART-A echo passed30s at92160B/s each way:2764800exact bytes, zero new Pico errors. Wi-Fi/STM32 DATA were excluded; this is isolation proof, not radio acceptance. Normal-path timing showed A input acknowledgement delayed up to1974ms, while Pico return ACK wait max10ms, output mutex/write max3ms and service gap18ms. Removing control-ACK UART activity from standalone-credit suppression was necessary but alone did not restore throughput. A stale-session QUERY guard also blocked startup indefinitely after undelivered old output;3.9.4 now serializes new host generations and prevents old DATA from preceding a new STATUS. Native tests cover old-window recovery, stale epoch rejection and credit eligibility.

Current A3.9.5 combines payload-only6ms/MSS batching with sendable-only credit deferral and session recovery. Its short921600 diagnostic is running. No new sustained pass is claimed yet. Programmer syscall tracing on3.9.4 showed CubeProgrammer aborting within milliseconds after reading late reset bytes0100 instead of a ROM ACK. B code enables its existing bounded pre-ACK boot-prefix filter for actual USB BOOT0 mode, preserving it across8E1 baud configuration; its build/native tests passed but it has not yet been installed.

###29 October standard wireless programming blocker resolved on3.9.6

The standard STM32CubeProgrammer2.21.0 read-only connection detected STM32L45x/L46x ID0462 and read application flash through Pico USB→A UART→Wi-Fi TCP→B UART. The subsequent saved-fixture download and verification passed; no SWD programming fallback was used. Download7.944s, verification7.527s, overall command18.129s. USB reset restored the application and256 exact echoed bytes. Evidence: [read-only](../experiments/pico-bridge/qualification-2026-10-29/SoC-Pico-filter/final-CubeProgrammer-readonly.json), [verified programming](../experiments/pico-bridge/qualification-2026-10-29/SoC-Pico-filter/final-programming.json).

A later binary probe found that transient RTS assertions can arm the new pre-ROM-ACK filter during normal8N1 operation. Candidate3.9.7 restricts prefix filtering to8E1; its native all256-byte regression passes. B/Pico images changed and are undergoing focused transport/programmer regression. The3.9.6 programmer pass remains scoped to its frozen hashes. No new sustained-load pass is claimed. AP channel1 failed authentication; original channel6 is restored. Vendor full-M4/NWP reset is integrated, but throughput improvement is not established.


##29 October —3.11 transport correction implemented; hardware pending

The user explicitly authorized implementation of the agreed Pico/A transport correction, with **no hardware testing or flashing during implementation**.

- **SOFTWARE COMPLETE:** Pico USB writes are limited to FIFO free space; A now admits complete radio blocks into an independent8KiB payload queue and drains it through a separate worker. A raw RX ring16KiB→8KiB funds that queue. The eight-packet ACK window, framing2, internal2M8N1, radio protocol6 and baud/reset/BOOT controls remain unchanged. New queue/session barriers prevent stale unsent payload crossing QUERY/reset boundaries.
- **SOFTWARE COMPLETE:** B UART receive overflow latches first-cause session failure20. A/B retain a188-byte bounded first-stall/overflow trace across core recovery. Counter capture includes the trace and new queue counts; qualification rejects mismatched admission/output counts, session discards and pending return payload.
- **SOFTWARE VERIFIED:**14 native transport/control tests, four demo-helper tests, Pico engine checks and seven AddressSanitizer/UndefinedBehaviorSanitizer tests passed. Actual TinyUSB/production Pico writer preserved8,388,608exact bytes under simulated host pauses. A queue/worker preserved16,809,312exact bytes across queue/sequence wrap. These are software-only tests, not radio/USB-controller measurements.
- **BUILDS COMPLETE:** Pico,A,B3.11.0 build successfully. Linker/stack reports and projected heap margins are archived. Original firmware/source/image SHA256 audit:872files unchanged. [Handoff/evidence](../experiments/pico-bridge/correction-2026-10-29/HANDOFF.md), [built bundle](../experiments/pico-bridge/correction-2026-10-29/bundle/build-manifest.json), [software verification](../experiments/pico-bridge/correction-2026-10-29/software-verification.json).
- **NOT INSTALLED:** hardware remains radios3.10.1/Pico3.9.7 and the unchanged STM32 fixture. Installed-image identities are preserved in `experiments/pico-bridge/correction-2026-10-29/pre-change/build-manifest.json`. The active experiment manifest/images now describe newly built3.11 files, not installed firmware. No board or serial/debug interface was accessed.
- **NEXT, ONLY AFTER HARDWARE RESUME:** install the matching new bundle; run the handoff's30s921600 test at92,160B/s each direction with exact bytes/hashes and all stage counters. Only on a pass proceed to600s. A failure requires reading the first captured timing/stop evidence before any further change. Do not rerun old COMPLETE acceptance sequences as setup.

The complete high-speed slowdown is **not claimed resolved by software checks**. Earlier hardware passes stay scoped to their preserved images. No new hardware throughput/endurance,24-hour, range or demonstration-readiness pass is claimed.


##29 October —3.11 installed; first921600 hardware gate FAILED

The user connected the hardware and authorized the planned installation/test. A,B,Pico were detected first; the requested Nucleo ST-LINK USB connection was then restored. All four boards were detected. Pico3.11UF2 installed; radio B440332624 and A440332408 flashed successfully. STM32 firmware was not reflashed. Image hashes matched the prepared3.11 bundle. [Installation](../experiments/pico-bridge/correction-2026-10-29/hardware-installation.json).

- **COMPLETE, connection check only:** Pico/A/B acknowledged the9216008N1 setting; STM32 fixture independently acknowledged921600/driver935064/BRR77. The complete path returned388,800exact test-prefix bytes. This verifies working connectivity, not sustained throughput.
- **FAILED, first30s full-capacity gate:** requested2,764,800bytes each direction at92,160B/s. Host accepted453,120bytes and returned388,800before a write timeout/no-echo timeout; elapsed10.10s. No first mismatch before the stall; Pico error increment0. B's retained first fault20 records159UART receive-ring overflow bytes, UART errors0, protocol errors0 and allocation failures0. A later recorded5s heartbeat timeout after B stopped. Do not treat post-reset zero counters as a pass.
- **CAPTURED:** B froze its new trace at first UART overflow; A/B progress and retained records are saved. A's returned radio hash matches B's TCP transmit hash. The long credit/delivery timing trigger remains unproven; the queue/USB correction has not met full-capacity acceptance. [Gate/counters](../experiments/pico-bridge/correction-2026-10-29/hardware-921600-30s.json), [status/analysis](../experiments/pico-bridge/correction-2026-10-29/hardware-status.json).
- **NOT STARTED:**600s test, as the prerequisite gate failed. No old completed acceptance test was repeated, no new firmware tuning was applied after this failure, and no hardware test is running. Current installed images remain3.11.0; source and original acceptance images remain preserved.

Next action is evidence-led isolation of the return-path timing, not a repetition of the failed endurance run or a claim that adding buffering solved the issue. Before any future necessary rerun, explain the exact diagnostic change or user request requiring it.24hours remains unqualified.


##29 October — directed diagnostic3.11.1 prepared

User explicitly requested continued resolution after the failed3.11 gate. The next checks are new diagnostic isolations, not repeated acceptance tests: B timed92160B/s RAM source through the existing return ring/radio/A/Pico path, first without forward DATA, then with forward DATA accepted at a B sink. Physical B/STM32 UART DATA are excluded in those two checks.3.11 buffers, baud settings, radio protocol and ACK window are unchanged. A additionally retains queue/ACK timing before recovery; prior3.11 counters were lost on reset.15 native tests and A/B builds pass. No client throughput pass is implied.3.11 source/images and failure report are preserved under correction-2026-10-29 and investigation-2026-10-29/pre-diagnostic.

**3.11.1 return-only diagnostic COMPLETE:** B RAM source→radio→A→Pico USB passed30s at92,160B/s,2,764,800exactbytes,source30,000ticks,Pico error increment0. Physical B/STM32 UART and forward DATA excluded. Proof: experiments/pico-bridge/investigation-2026-10-29/return-only-30s.json. This is isolation evidence, not complete UART acceptance. Next distinct check adds simultaneous forward traffic to the same configuration.

**3.11.1 simultaneous RAM-source/sink diagnostic FAILED:** failure reproduced with physical STM32 UART DATA excluded. A retained queue peak8192/enqueue wait16ms; cumulative Pico ACK wait increased only22ms over this run (1413→1435ms), so long Pico ACK waits do not account for the failure. Source/sink evidence: investigation-2026-10-29/duplex-radio-30s.json. A worker was found to sleep one tick after every256-byte packet even with ready queued data.3.11.2 removes that unconditional sleep; empty/ACK-blocked paths still sleep and the8-frame window bounds bursts.16 native tests, including actual-worker no-sleep-while-ready/window-fairness checks, and A build passed. The next rerun repeats only the failed simultaneous diagnostic because this specific scheduling path changed; no buffers,window,baud or framing are changed.


##29 October — measured duplex bottleneck and3.11.3 correction

3.11.2 simultaneous RAM-source/sink diagnostic delivered2,764,800exact return bytes at92,160B/s with zero new Pico errors and no B overflow. Forward delivery was exact but took32.393s instead of30s, so this is not a full-rate acceptance pass. A input ACK delay reached38ms; the original2048-byte input window covers only22.22ms at92,160B/s. Evidence: investigation-2026-10-29/drain-duplex-radio-30s.json.

3.11.3 retains the drain-ready worker and expands only the private Pico/A window to24x256bytes(66.67ms coverage). Private framing3 uses COBS: DATA maximum269wirebytes, ACK maximum12.24*(269+12)+512=7256<8192, fitting existing raw queues even for worst-case payloads. Radio protocol6,2M internal baud,8KiB return queue and target/control settings are unchanged. This supersedes the earlier preserve-window/framing restriction because the measured ACK delay exceeds that window. All17 native tests and Pico/A/B builds pass. Linker-measured C-heap spare relative to historical peaks is A3272/B3456bytes; runtime check pending. Source/images preserved in pre-window-correction and window-bundle.

The new image set requires one new physical-path92160030s gate because scheduling/window/framing changed. Proceed to600s only after exact bytes, offered rate and stage/error checks pass. No old completed acceptance sequence is restarted as setup. Installation and hardware result pending.


3.11.3 physical30s gate FAILED:86,528host accepted,11,520exact prefix returned,zero new Pico errors. B first-cause20/27overflowbytes; retained credit trace shows>250ms radio/credit pause, while A output queue was drained and Pico ACK wait0. Native17tests and8ASan/UBSanchecks passed; hardware acceptance failed.

3.11.4 corrects a vendor configuration discrepancy: TCP TX1+RX1+high-performance RX1 requires totalTCP3, plusUDP1,total4. Current3.11.3 usedTCP2,total3. Original passing3.7.2 reservedTCP3. Official multithread_sockets app/readme confirm three TCP roles. Only A/B reservation/version changes; Pico3.11.3,radio protocol6,buffers and rates unchanged. New30s gate required specifically to validate this reservation correction. No600s run started.

3.11.4 physical30s delivered2,764,800exact bytes each way,zero new Pico/radio/STM32errors,returnqueue counts exact. Offered-rate gate FAILED:writer30.781s. TCP reservation correction removed overflow in this comparison. A output mutex wait reached29ms,inputACK53ms,servicegap37ms.3.11.5 adds an equal-priority yield after each successful output packet to let pending input/ACK/mutex waiters run; no timed pause,packet/window/buffers/radio changes. Actual-worker test now checks one yield per packet and no sleep before blocked. Rerun only new30s gate after this scheduling correction.

3.11.5 gate FAILED again with B overflow129bytes, while A queue and Pico output drained; socket-reservation-only change therefore does not establish the remaining cause. The earlier3.11.4exact run still missed offered rate.3.11.6 restores the original passing radio send/credit scheduling and TCP3 allocation: cumulative credits piggybacked on DATA or sent when no sendable DATA remains; no UDP credit socket. Pico framing3/window24 and independent A drain worker with fairness yield retained. RadioDATA/controlformat6 and all baud/reset controls unchanged. This comparison removes the independent UDP scheduling introduced during the failed investigation and returns that layer to the proven baseline rather than adding buffers. Only the new30s physical gate pending.

3.11.6 restored TCP-credit physical30s test delivered2,764,800exactbytes eachway;zero new measured errors,writer31.948s so offered-rateFAIL. Bcreditwaitmax935ms,AinputACKmax961ms; Aoutputmutexmax5ms and PicoACKwaitmax11ms.3.11.7 instrumentation-only generatedSDKadapter now records M4->NWP queue readiness wait and busTX/RX counters. Buffers,scheduling,protocol,rates unchanged. A/Bbuilds passed. Next30s directed measurement distinguishes SDK bus stall from post-NWP/radio delay; not a completed acceptance rerun.

3.11.7 physical 921600 30s gate COMPLETE: 2,764,800 exact bytes each direction, writer29.996s, zero new errors, all eight stage counts/hashes and return queue verified. Evidence: experiments/pico-bridge/investigation-2026-10-29/bus-final-921600-30s.json and -verification.json. The user-authorized600s test follows because this changed image passed its gate. No 24h claim.

3.11.7 600s requested full-rate test FAILED: at600s host sent54,236,672 and received54,181,025 bytes; final stop610s sent55,161,856 and received55,105,729, no mismatch in the received prefix, no new Pico error. Required55,296,000 each way was not reached before the tester deadline. B maximum radio-credit wait3033ms; A maximum input ACK wait3878ms. B instrumented maximum M4-to-NWP queue wait0ms for this run, suggesting remaining delay is after that queue; this does not establish its precise cause. A bus maximum5708ms predates the test and cannot be attributed to it. No zero-loss or full-rate600s pass claimed. Evidence: experiments/pico-bridge/investigation-2026-10-29/bus-921600-600s.json. Current installed A/B3.11.7/Pico3.11.3, STM32 unchanged. No test is running. Original872-file baseline audit unchanged.

29 October: User explicitly prioritizes exact delivery and accepts delay. The prior test forcibly stopped after600+10 seconds while bytes were still in flight; its matching prefix does not establish completion. Test helper now offers --allow-delay with up to120s extra and15s inactivity guard, without changing the strict full-rate default. New physical test sends the same nominal 600-second amount, drains completely, checks exact bytes, all stage counts/hashes and errors; rate is reported separately. This rerun tests the new delayed-delivery acceptance criterion explicitly requested by the user.

29 October — delayed-delivery921600 test COMPLETE for this image and bench: A/B3.11.7, Pico3.11.3, unchanged STM32 fixture.55,296,000exact bytes sent and received, SHA256 equal, writer602.517s, complete drain603.251s, Pico error increment0; all eight stage counts/FNV hashes and all A/B/STM32 error increments verified0. Full nominal-rate threshold failed; acceptance here explicitly permits delay. [Host result](../experiments/pico-bridge/investigation-2026-10-29/delayed-921600-600s-connected.json); [stage verification](../experiments/pico-bridge/investigation-2026-10-29/delayed-921600-600s-connected-verification.json). This does not establish zero loss on link drops, uncontrolled target transmission, other conditions or24h. No test is running.

29 October — user requested full-speed correction and rerun of all tests. Fresh3.11.7 strict30s test transferred2,764,800exactbytes each direction but took32.314s: full-rate FAIL. A batching counters advanced1920full/0partial packets, excluding short forward batches as the observed cause. B credit-wait cumulative time increased4397ms during the run; historical maximum1013ms, reported RSSI -31dBm/status0. The exact mechanism causing radio/credit stalls remains unproven; a good RSSI does not rule out contention or packet loss. Evidence: experiments/pico-bridge/investigation-2026-10-29/batching-current-30s.json. No firmware was changed or flashed in this check; no additional tests are running.

29 October — continued full-speed investigation explicitly requested. All four boards detected. A/B3.11.7 remain installed; no firmware change. A live SWD queue trace (diagnostic only, not acceptance) delivered2,764,800exactbytes each way in30.244s with writer29.996s. A longer120s trace is collecting the intermittent stall; no complete-suite pass is claimed. Evidence: experiments/pico-bridge/investigation-2026-10-29/queue-trace/run.json.

30 October: 120s synchronized trace reproduced delay (11,059,200exact bytes, writer122.702s). During one stall A radio callbacks stopped1023ms with empty application queues; B had handed return packets to NWP. Resume followed A heartbeat; this correlation does not establish NWP internals. Candidate3.11.8 repeats earned cumulative credit every20ms only while outgoing window blocked and RX/TX idle.17 native tests and A/B builds passed. Install and strict30s gate authorized by user; originals archived in investigation-2026-10-30/credit-refresh.

30 October —3.11.8 credit-refresh candidate REJECTED. Strict30s gate failed:501,760host accepted/463,680received by40.37s, no mismatch in prefix but unfinished and extra1440bytes. No new measured radio/UART overflow or protocol errors; no completed zero-loss claim. Frequent cumulative credit refresh did not resolve NWP delivery stall. Candidate source/images/logs archived at investigation-2026-10-30/credit-refresh. Restoring exact3.11.7 source/images and board firmware; full-rate and requested functional rerun remain pending, no600s test started.

Restoration confirmed: Commander reported successful flash for both A and B with preserved3.11.7 images. Pico/STM32 unchanged. No test running.

30 October — over-air diagnostic blocked before execution by automatic approval review. Laptop has only one Wi-Fi radio, currently providing Internet on5GHz; prototype uses channel6. Proposed bounded capture would temporarily disconnect laptop Wi-Fi, capture only the prototype station MAC, then restore the original connection. Review requires explicit user approval for interruption and payload interception. No network change or packet capture executed. A/B remain restored3.11.7; no test running. This capture is needed to distinguish TCP retransmission/window behavior from NWP delivery delay without another speculative firmware change.

30 October — user explicitly approved temporary Wi-Fi interruption/prototype capture. Initial attempt failed creating monitor interface (Linux EPERM); finally block restored saved Wi-Fi successfully. No capture or payload test ran. Passwordless sudo unavailable. Desktop pkexec administrator authentication requested; waiting for user approval in OS dialog.

30 October capture recovery: user removed monitor interface; readback confirms normal5GHz Wi-Fi/default route and no tcpdump. Prior capture test exited before payload with missing arm-none-eabi-nm under pkexec PATH; zero captured packets. Revised capture sets explicit toolchain PATH, keeps capture UID root, imposes90s capture limit and unconditional nested Wi-Fi restoration. No throughput pass claimed.

30 October — corrected capture v2 restored Wi-Fi automatically but captured zero packets. Its payload test failed (write timeout, incomplete stream); no loss/completion claim. Prototype AP scan confirms channel6/BSSID08:dd:eb:cf:49:18. Capture v3 explicitly requests otherbss monitor reception and filters the verified AP address; will skip payload test unless packets are captured first. Awaiting desktop administrator authentication. Evidence under investigation-2026-10-30/air-capture-v2.

## 30 October — WD-v1 rename and delay-tolerant requirement run (A/B 3.11.7-WD-v1, Pico 3.11.3, STM32 fixture 3.8.0)

User accepted extra delay but no data loss. Only firmware change from 3.11.7: SSID `test` → `WD-v1` (B had joined a neighbouring open network also named `test` on channel 6). A UDP transport attempt (3.12.x) was built, not stable on hardware, and rolled back; source archived in `experiments/pico-bridge/claude-2026-10-30/udp-attempt/`. Pico was restarted (same UF2) after a latched UART fault. Evidence: `experiments/pico-bridge/claude-2026-10-30/req-*.json`.

| Check | Result |
| --- | --- |
| 921600 full rate 300 s | **PASS exact:** 27,648,000 bytes, SHA match, 89.4 KB/s achieved; round-trip delay under load median 0.62 s, p99 1.26 s, max 2.45 s |
| 115200 full rate 300 s | **PASS exact:** 3,456,000 bytes; delay median 0.30 s, max 0.58 s |
| Six-rate USB baud forwarding | **PASS:** 9600–921600, 4096 exact bytes each, one open COM |
| USB DTR/RTS → ROM | **PASS:** GET_ID 0462, app restored with exact echo |
| Idle latency 1 byte | 921600 median 19.5 ms (max 105 ms); 115200 median 19.4 ms (max 139 ms) |
| Wi-Fi power policy | **PASS** A/B high-performance |
| Reconnect, same COM | **PASS:** B 10.9 s, A 27.7 s, 4096 exact bytes after each |

Delay under continuous full load comes from ~1 s TCP retransmit stalls in the NWP plus only ~0.8% UART headroom; data is never lost, it queues. 24 h and wall range not run.

## 30 October — DTR/RTS auto-reset mapping (Pico 3.12.0) — delivery default

Problem: every terminal asserts DTR+RTS on open; the old direct mapping (DTR=reset, RTS=BOOT0) froze the STM32 / entered ROM on every connect. Pico 3.12.0 now uses the ESP32-style mapping with a 10 ms settle filter. A 3.11.7-WD-v1 and B 3.11.9-rejoin-500ms unchanged. Evidence: `experiments/pico-bridge/claude-2026-10-30/dtr-test*`.

| DTR | RTS | STM32 |
| --- | --- | --- |
| off | off | runs |
| on | on | runs (normal terminal) |
| off | on | reset held |
| on | off | BOOT0 high, reset released |

| Check | Result |
| --- | --- |
| Terminal-style open (DTR+RTS asserted), no release step | **PASS** 5/5 exact echoes, no target reset |
| Six-rate baud matrix | **PASS** 4096 exact bytes each |
| ROM entry via new sequence | **PASS** GET_ID 0462, app restored with exact echo |
| STM32CubeProgrammer 2.21.0 wireless program + verify | **PASS** download 9.5 s, verify 9.4 s, no SWD; app restored, 256-byte exact echo |
| `demo-support.py reset` | **PASS** app running, terminal echo exact |

Client flashing procedure: `python3 docs/demo-support.py boot`, then `STM32_Programmer_CLI -c port=<Pico data port> br=115200 P=EVEN dtr=high rts=low -d <file> -v`, then `python3 docs/demo-support.py reset`. Tools that set DTR and RTS in separate calls more than 10 ms apart may pass through the run state; set both together (TIOCMSET) as the helper does. `demo-support.py release` is no longer needed.

## 30 October — automatic bootloader selection (Pico 3.12.1) — delivery default

The client no longer runs any command. USB line coding selects the target mode: opening the port at **8E1** (the STM32 ROM bootloader's format, as STM32CubeProgrammer does) makes the Pico reset the STM32 with BOOT0 high; opening at **8N1** (any terminal) resets it back into the application. DTR/RTS keep the auto-reset mapping (equal lines = run) and are ignored while the bootloader is selected. A 3.11.7-WD-v1 and B 3.11.9-rejoin-500ms unchanged. Evidence: `experiments/pico-bridge/claude-2026-10-30/autoboot-*`.

| Check | Result |
| --- | --- |
| Plain `STM32_Programmer_CLI -c port=<Pico data> br=115200 P=EVEN` program + verify, no helper, no DTR/RTS options | **PASS** chip 0x462, download 9.8 s, verify 9.8 s, "Download verified successfully"; app restored with exact echo |
| Six-rate baud matrix | **PASS** 4096 exact bytes each |
| Automatic ROM entry on 8E1 | **PASS** GET_ID 0462; 8N1 restored application with exact echo |
| Terminal-style open (DTR+RTS asserted, 8N1) | **PASS** 3/3 exact echoes |

Limitation: a target application that itself uses 8E1 would enter the bootloader when a terminal opens at 8E1. The first feature-test attempt after the Pico reinstall failed before any step (fixture silent during restart); the immediate rerun passed.

30 October — user requested only921600 reliability resolution and a3600s test after Claude changes. Current manifest A/B3.11.12-hb15s/Pico3.12.1 retained; no firmware edits or flashes. All four boards detected, no competing longrun process. Started strict3600s at92160B/s with before/after diagnostics: qualification-2026-10-30/921600-one-hour.json (live log alongside). First30s sent1,869,312/received1,828,129 matching prefix, below nominal rate. Test still running; no pass claimed. Claude longrun failures at561s,1000s,719s include actual mismatches and must not be described as delay-only success.

30 October — latest acceptance clarification: user accepts a small delay but requires zero missing/corrupted/duplicated bytes at921600. Preserve exact comparisons, final drained byte counts/hashes and error checks; report throughput/delay independently. Current strict one-hour process remains running unchanged; rate-only failure is distinct from integrity failure. A deadline with bytes still queued is incomplete, never proof of data loss or a zero-loss pass. At90s the received prefix5,979,553bytes matched; no completion claim.

30 October — requested one-hour921600 run stopped at1934.38s (32m14s): write timeout and5s no-echo timeout.146,841,600accepted host bytes;146,789,888received, no mismatch in received prefix;51,712outstanding when stopped. No new measured radio/STM32 errors/overflow/timeouts or Pico error increment; A/B stop reason0. Incomplete, not a zero-loss pass and not proof those outstanding bytes were lost. Strict tester retained its earlier short timeouts despite subsequent user acceptance of delay; next diagnosis must distinguish transient backpressure from session loss and use bounded complete draining. Evidence: qualification-2026-10-30/921600-one-hour.json. No test running; no firmware changes.
