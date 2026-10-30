# Raspberry Pi Pico — USB bridge

Firmware 3.12.1 for RP2040. Appears on the laptop as two USB serial ports:

- **UART data** (interface 0): the "cable". Open it in any terminal at the target's baud.
- **Bridge status** (interface 2): send `STATUS` + newline for baud, DTR/RTS, link state and counters.

It forwards the laptop's baud/parity and DTR/RTS to board B over a framed 2 Mbaud UART to board A (GP0 TX, GP1 RX).

## Target control (DTR/RTS)

| DTR | RTS | Target |
| --- | --- | --- |
| off | off | runs |
| on | on | runs (what terminals set on open) |
| off | on | reset held |
| on | off | BOOT0 high, reset released |

Opening the port at **8E1** (as STM32CubeProgrammer does) automatically resets the STM32 into its ROM bootloader; reopening at **8N1** resets it back into the application. Changes are applied after the lines are stable for 10 ms.

## Flash the prebuilt image

Hold BOOTSEL while plugging in the Pico, then copy `images/pico-wireless-uart.uf2` to the `RPI-RP2` drive. A running Pico can also be put in BOOTSEL by sending `BOOTSEL` on the status port.

## Build

Needs Pico SDK 2.2.0 (with TinyUSB) and arm-none-eabi-gcc.

```bash
cmake -S pico -B pico/build -DPICO_BOARD=pico -DPICO_SDK_PATH=/path/to/pico-sdk -DCMAKE_BUILD_TYPE=Release
cmake --build pico/build -j4
picotool uf2 convert pico/build/wireless_uart_pico.elf pico-wireless-uart.uf2 --family rp2040
```

## Native tests

```bash
cc -O1 -Wall -Wextra -fsanitize=address,undefined -Ipico -Ifirmware -o test_engine tests/test_engine.c pico/bridge_engine.c && ./test_engine
cc -O1 -Wall -Wextra -fsanitize=address,undefined -Ipico -Ifirmware -o test_modem tests/test_modem_mapping.c pico/bridge_engine.c && ./test_modem
```
