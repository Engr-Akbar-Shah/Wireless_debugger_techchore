# Board A — SiWx917 access point (laptop side)

Firmware 3.11.12-hb15s for SiWx917 BRD2605A. Creates Wi-Fi AP **WD-v1** (2.4 GHz, channel 6, 192.168.10.10), accepts board B on TCP 5005, and talks to the Pico over a framed 2 Mbaud UART (GPIO29 RX, GPIO30 TX).

## Layout

- `firmware/Debugger-SiW917_AccessPoint/` — Simplicity Studio project (`app.c`, `host_uart.c`, `config/`, `.slcp`).
- `firmware/*.h`, `bridge_sdk_fixes.cmake` — shared bridge code used by A and B (transport, Pico link, diagnostics).
- `images/board-A-pico.rps` — prebuilt image; `board-A-pico.out` — matching ELF (symbols for diagnostics).

## Flash the prebuilt image

```bash
commander flash images/board-A-pico.rps --device SiWG917M111MGTBA --serialno <A J-Link serial>
```

## Build

Needs Simplicity Studio 6 with Simplicity SDK 2025.12.3 and WiSeConnect 3 SDK 4.0.2. The SDK folders are not in this repo.

1. Open `firmware/Debugger-SiW917_AccessPoint/Debugger-SiW917_AccessPoint.slcp` in Simplicity Studio and regenerate (the generated cmake files contain the original machine's absolute paths).
2. Build with the `cmake_gcc` / `default_config` preset: `cd cmake_gcc && cmake --workflow --preset project`.

## Notes

- Network name and passphrase are in `config/sl_net_default_values.h` (bench default passphrase `12345678`; change for real use and keep B in sync).
- Key timeouts: PC/USB backpressure tolerated 10 s; radio link considered dead after 15 s without frames.
