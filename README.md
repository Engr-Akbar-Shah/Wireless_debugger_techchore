# Board B — SiWx917 station (target side)

Firmware 3.11.12-hb15s for SiWx917 BRD2605A. Joins Wi-Fi **WD-v1**, connects to A (192.168.10.10:5005), drives the target UART (USART0: GPIO30 TX, GPIO29 RX) and the target's NRST (GPIO25) and BOOT0 (GPIO26). The USB VCOM prints a diagnostic log at 115200.

## Layout

- `firmware/Debugger-SiW917_station/` — Simplicity Studio project (`app.c`, `target_uart.c`, `target_control.h`, `config/`, `.slcp`).
- `firmware/*.h`, `bridge_sdk_fixes.cmake` — shared bridge code used by A and B.
- `images/board-B-pico.rps` — prebuilt image; `board-B-pico.out` — matching ELF.

## Flash the prebuilt image

```bash
commander flash images/board-B-pico.rps --device SiWG917M111MGTBA --serialno <B J-Link serial>
```

## Build

Same as board A: Simplicity Studio 6, Simplicity SDK 2025.12.3, WiSeConnect 3 SDK 4.0.2. Open the `.slcp`, regenerate, then `cd cmake_gcc && cmake --workflow --preset project`.

## Notes

- Target UART baud follows the laptop's selected baud (9600–921600, 8N1; 115200 8E1 for the STM32 bootloader).
- NRST is driven open-drain (low or released, never high).
- Rejoin retry delay 0.5 s after the AP disappears.
