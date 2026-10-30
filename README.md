# STM32 echo test fixture (NUCLEO-L452RE)

Fixture 3.8.0 used as the target in all bridge tests. USART1 (PA9 TX / D8, PA10 RX / D2) echoes every byte with circular RX DMA and queued TX DMA. The ST-LINK virtual COM port (USART2, 115200) takes commands:

| Command | Reply |
| --- | --- |
| `STATUS` | `OK <nominal baud> <driver baud> <BRR>` |
| `BAUD <rate>` | sets USART1 (9600, 57600, 115200, 230400, 460800, 921600); `ERR busy` while data is moving |

After any reset the fixture starts at 921600. The driver baud is about 1.46% above nominal for TX headroom.

## Flash

```bash
STM32_Programmer_CLI -c port=SWD mode=UR reset=HWrst -d images/stm32-baud-fixture.elf -v -hardRst
```

## Build

Open `test-fixtures/` in STM32CubeIDE (project files `.project`, `.cproject`, `test-fixtures.ioc`) and build the Debug configuration. HAL/CMSIS drivers are included under `Drivers/`.
