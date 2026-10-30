# Debugger-SiW917_AccessPoint

Current role: **laptop AP / TCP server**.

- Payload: onboard USB VCOM, 115200 8N1.
- Diagnostics: No console output on the payload port.
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
