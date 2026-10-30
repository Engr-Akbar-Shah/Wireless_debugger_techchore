# Debugger-SiW917_station

Current role: **target station / TCP client**.

**Current bench mode: `TCP_ECHO_TEST=0`.** TCP bytes go to the external UART and received UART bytes go back over TCP. Board B is J-Link serial **440332624**. The earlier [TCP-only test](../docs/tcp-only-test.md) passed before switching to this mode.

- Payload: USART0: GPIO30 TX / GPIO29 RX, 115200 8N1.
- Diagnostics: USB VCOM at 115200 is diagnostic only.
- Bench network: existing paired AP/station profiles, TCP port 5005.
- First milestone: short messages and loopback, then NUCLEO-L452RE echo.

Read the [first-test guide](../docs/first-uart-test.md) before wiring or flashing.
See the [workspace README](../README.md) for cleanup/backup information.

Build from this project's `cmake_gcc` directory:

```bash
cmake --workflow --preset project
```

Flash this project's postprocessed RPS with the Silicon Labs project flash action.
Keep the project-local SDKs, `config`, `autogen`, and generation metadata.

The firmware is an initial fixed-baud prototype; high-rate qualification, flow control,
lossless recovery, automatic baud changes, and reset/bootloader control are later phases.
