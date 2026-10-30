####################################################################
# Automatically-generated file. Do not edit!                       #
####################################################################

set(SDK_PATH "/home/ghost/.silabs/slt/installs/conan/p/simpl35774a752829c/p")
set(COPIED_SDK_PATH "simplicity_sdk_2025.12.3")
set(PKG_PATH "/home/ghost/.silabs/slt/installs")

add_library(slc OBJECT
    "../${COPIED_SDK_PATH}/cmsis/RTOS2/Source/os_systick.c"
    "../${COPIED_SDK_PATH}/cmsis_common/platform/common/src/sl_cmsis_os2_common.c"
    "../${COPIED_SDK_PATH}/cmsis_common/platform/common/src/sli_cmsis_os2_ext_task_register.c"
    "../${COPIED_SDK_PATH}/freertos/cmsis/Source/cmsis_os2.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/croutine.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/event_groups.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/list.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/portable/GCC/ARM_CM4F/port.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/portable/MemMang/heap_4.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/queue.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/stream_buffer.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/tasks.c"
    "../${COPIED_SDK_PATH}/freertos/kernel/timers.c"
    "../${COPIED_SDK_PATH}/platform_common/platform/common/src/sl_assert.c"
    "../${COPIED_SDK_PATH}/platform_common/platform/common/src/sl_slist.c"
    "../${COPIED_SDK_PATH}/platform_common/platform/common/src/sl_string.c"
    "../${COPIED_SDK_PATH}/platform_common/platform/common/src/sl_syscalls.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/common/src/sl_core_cortexm.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/mem_pool/src/sl_mem_pool.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/src/rtos/main_retarget.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/src/sl_main_init.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/src/sl_main_init_memory.c"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/src/sl_main_kernel.c"
    "../app.c"
    "../autogen/sl_event_handler.c"
    "../main.c"
    "../wiseconnect3_sdk_4.0.2/components/board/silabs/src/rsi_board.c"
    "../wiseconnect3_sdk_4.0.2/components/common/src/malloc_thread_safety.c"
    "../wiseconnect3_sdk_4.0.2/components/common/src/sl_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/iPMU_prog/iPMU_dotc/ipmu_apis.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/iPMU_prog/iPMU_dotc/rsi_system_config_917.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/rsi_deepsleep_soc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/rsi_ps_ram_func.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/startup_si91x.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/src/system_si91x.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/common/src/rsi_debug.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/common/src/sl_si91x_stack_object_declare.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/common/src/syscalls.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/config/src/rsi_nvic_priorities_config.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/cmsis_driver/UDMA.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/cmsis_driver/USART.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/aux_reference_volt_config.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/clock_update.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_adc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_crc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_dac.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_egpio.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_opamp.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_udma.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_udma_wrapper.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/src/rsi_usart.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/service/clock_manager/src/sl_si91x_clock_manager.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_bod.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_ipmu.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_pll.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_rtc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_temp_sensor.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_time_period.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/src/rsi_ulpss_clk.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/unified_api/src/sl_si91x_adc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/unified_api/src/sl_si91x_bjt_temperature_sensor.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/unified_api/src/sl_si91x_dma.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/unified_api/src/sl_si91x_usart.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/rsi_hal_mcu_m4_ram.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/rsi_hal_mcu_m4_rom.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/sl_platform.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/sl_platform_wireless.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/sl_si91x_bus.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/sl_si91x_timer.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/src/sli_siwx917_soc.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/asynchronous_socket/src/sl_si91x_socket.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/errno/src/sl_si91x_errno.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/firmware_upgrade/firmware_upgradation.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/host_mcu/si91x/siwx917_soc_ncp_host.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/icmp/sl_net_ping.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/memory/mem_pool_buffer_quota.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sl_net_rsi_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x_callback_framework.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sl_net_si91x_integration_handler.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sl_si91x_net_internal_stack.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/src/sli_net_si91x_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/socket/src/sl_si91x_socket_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/src/sl_rsi_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/src/sl_si91x_driver.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/src/sli_wifi_memory_manager.c"
    "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/src/sli_wifi_power_profile.c"
    "../wiseconnect3_sdk_4.0.2/components/protocol/wifi/si91x/sl_wifi.c"
    "../wiseconnect3_sdk_4.0.2/components/protocol/wifi/src/sl_wifi_basic_credentials.c"
    "../wiseconnect3_sdk_4.0.2/components/protocol/wifi/src/sl_wifi_callback_framework.c"
    "../wiseconnect3_sdk_4.0.2/components/protocol/wifi/src/sli_wifi_callback_framework.c"
    "../wiseconnect3_sdk_4.0.2/components/service/bsd_socket/si91x_socket/sl_si91x_bsd_socket.c"
    "../wiseconnect3_sdk_4.0.2/components/service/network_manager/src/sl_net.c"
    "../wiseconnect3_sdk_4.0.2/components/service/network_manager/src/sl_net_basic_certificate_store.c"
    "../wiseconnect3_sdk_4.0.2/components/service/network_manager/src/sl_net_basic_profiles.c"
    "../wiseconnect3_sdk_4.0.2/components/service/network_manager/src/sl_net_credentials.c"
    "../wiseconnect3_sdk_4.0.2/components/service/network_manager/src/sli_net_common_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/service/sl_websocket_client/src/sl_websocket_client.c"
    "../wiseconnect3_sdk_4.0.2/components/service/sl_websocket_client/src/sli_websocket_client_sync.c"
    "../wiseconnect3_sdk_4.0.2/components/sli_buffer_manager/src/sli_buffer_manager.c"
    "../wiseconnect3_sdk_4.0.2/components/sli_si91x_wifi_event_handler/src/sli_si91x_wifi_event_handler.c"
    "../wiseconnect3_sdk_4.0.2/components/sli_wifi/src/sli_wifi.c"
    "../wiseconnect3_sdk_4.0.2/components/sli_wifi/src/sli_wifi_utility.c"
    "../wiseconnect3_sdk_4.0.2/components/sli_wifi_command_engine/src/sli_wifi_command_engine.c"
    "../wiseconnect3_sdk_4.0.2/third_party/iot_socket/source/bsd_socket/iot_socket.c"
)

target_include_directories(slc PUBLIC
   "../config"
   "../autogen"
   "../."
    "../${COPIED_SDK_PATH}/platform_common/platform/common/inc"
    "../${COPIED_SDK_PATH}/cmsis_common/platform/common/inc"
    "../${COPIED_SDK_PATH}/cmsis/Core/Include"
    "../${COPIED_SDK_PATH}/cmsis/RTOS2/Include"
    "../${COPIED_SDK_PATH}/platform_core/platform/emlib/inc"
    "../${COPIED_SDK_PATH}/freertos/kernel/include"
    "../${COPIED_SDK_PATH}/freertos/cmsis/Include"
    "../${COPIED_SDK_PATH}/freertos/kernel/portable/GCC/ARM_CM4F"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/mem_pool/inc"
    "../${COPIED_SDK_PATH}/platform_core/platform/common/inc"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/inc"
    "../${COPIED_SDK_PATH}/platform_core/platform/service/sl_main/src"
   "../wiseconnect3_sdk_4.0.2/components/protocol/wifi/inc"
   "../wiseconnect3_sdk_4.0.2/components/service/bsd_socket/inc"
   "../wiseconnect3_sdk_4.0.2/components/service/bsd_socket/si91x_socket"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/cmsis_driver/CMSIS/Driver/Include"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/cmsis_driver"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/common/inc"
   "../wiseconnect3_sdk_4.0.2/third_party/iot_socket/include"
   "../wiseconnect3_sdk_4.0.2/third_party/iot_socket/silabs"
   "../wiseconnect3_sdk_4.0.2/components/service/network_manager/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/rom_driver/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/chip/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/peripheral_drivers/inc"
   "../wiseconnect3_sdk_4.0.2/components/board/silabs/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/core/config"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/service/sl_log/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/unified_api/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/service/clock_manager/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/asynchronous_socket/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/errno/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/firmware_upgrade"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/sl_net/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/icmp"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/socket/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/inc/mqtt/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/wireless/ahb_interface/inc"
   "../wiseconnect3_sdk_4.0.2/components/service/sl_websocket_client/inc"
   "../wiseconnect3_sdk_4.0.2/components/sli_buffer_manager/inc"
   "../wiseconnect3_sdk_4.0.2/components/sli_wifi/inc"
   "../wiseconnect3_sdk_4.0.2/components/sli_wifi_command_engine/inc"
   "../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/systemlevel/inc"
   "../wiseconnect3_sdk_4.0.2/resources/defaults"
   "../wiseconnect3_sdk_4.0.2/resources/certificates"
   "../wiseconnect3_sdk_4.0.2/resources/html"
   "../wiseconnect3_sdk_4.0.2/resources/other"
   "../wiseconnect3_sdk_4.0.2/components/common/inc"
)

target_compile_definitions(slc PUBLIC
    "SL_SI91X_PRINT_DBG_LOG=1"
    "configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS=2"
    "SL_COMPONENT_CATALOG_PRESENT=1"
    "SL_CODE_COMPONENT_FREERTOS_KERNEL=freertos_kernel"
    "SL_CODE_COMPONENT_CORE=core"
    "SIWG917M111MGTBA=1"
    "SLI_SI917=1"
    "SLI_SI917B0=1"
    "SLI_SI91X_MCU_ENABLE_FLASH_BASED_EXECUTION=1"
    "SLI_SI91X_MCU_EXTERNAL_LDO_FOR_PSRAM=1"
    "SI917_DEVKIT=1"
    "SLI_SI91X_MCU_COMMON_FLASH_MODE=1"
    "SLI_SI91X_MCU_CONFIG_RADIO_BOARD_BASE_VER=1"
    "SLI_SI91X_MCU_CONFIG_RADIO_BOARD_VER2=1"
    "SL_BOARD_NAME=\"BRD2605A\""
    "SL_BOARD_REV=\"A02\""
    "__FREERTOS_OS_WISECONNECT=1"
    "SL_NET_COMPONENT_INCLUDED=1"
    "__STATIC_INLINE=static inline"
    "CLOCK_ROMDRIVER_PRESENT=1"
    "ULPSS_CLOCK_ROMDRIVER_PRESENT=1"
    "SL_SI91X_BOARD_INIT=1"
    "SRAM_BASE=0x0cUL"
    "SRAM_SIZE=0x2fc00UL"
    "SLI_CODE_CLASSIFICATION_DISABLE=1"
    "SLI_SI91X_MCU_ENABLE_IPMU_APIS=1"
    "SL_SI91X_SOC_MODE=1"
    "CRC_ROMDRIVER_PRESENT=1"
    "DEBUG_ENABLE=1"
    "DEBUG_UART=1"
    "ENABLE_DEBUG_MODULE=1"
    "SL_SI91X_SI917_RAM_MEM_CONFIG=1"
    "UDMA_ROMDRIVER_PRESENT=1"
    "SLI_SI91X_OFFLOAD_NETWORK_STACK=1"
    "SI917=1"
    "SLI_SI91X_ENABLE_OS=1"
    "SLI_SI91X_MCU_INTERFACE=1"
    "TA_DEEP_SLEEP_COMMON_FLASH=1"
    "SLI_SI91X_SOCKETS=1"
    "SL_SI91X_REQUIRES_INTF_PLL=1"
    "SL_SI91X_USART_DMA=1"
    "USART_MODULE=1"
    "PLL_ROMDRIVER_PRESENT=1"
    "SL_WIFI_COMPONENT_INCLUDED=1"
)

target_link_libraries(slc PUBLIC
    "-Wl,--start-group"
    "gcc"
    "c"
    "m"
    "nosys"
    "-Wl,--end-group"
)
target_compile_options(slc PUBLIC
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m4>
    $<$<COMPILE_LANGUAGE:C>:-mthumb>
    $<$<COMPILE_LANGUAGE:C>:-mfpu=fpv4-sp-d16>
    $<$<COMPILE_LANGUAGE:C>:-mfloat-abi=softfp>
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Os>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:C>:-g>
    $<$<COMPILE_LANGUAGE:C>:--specs=nano.specs>
    "$<$<COMPILE_LANGUAGE:C>:SHELL:-Wall -Werror>"
    $<$<COMPILE_LANGUAGE:C>:-Wno-error=deprecated-declarations>
    "$<$<COMPILE_LANGUAGE:C>:SHELL:-Wall -Werror -Wno-error=deprecated-declarations>"
    $<$<COMPILE_LANGUAGE:C>:-mcpu=cortex-m4>
    $<$<COMPILE_LANGUAGE:C>:-fno-lto>
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m4>
    $<$<COMPILE_LANGUAGE:CXX>:-mthumb>
    $<$<COMPILE_LANGUAGE:CXX>:-mfpu=fpv4-sp-d16>
    $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=softfp>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Os>
    $<$<COMPILE_LANGUAGE:CXX>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:CXX>:-fomit-frame-pointer>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
    $<$<COMPILE_LANGUAGE:CXX>:--specs=nano.specs>
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-Wall -Werror>"
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-declarations>
    "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-Wall -Werror -Wno-error=deprecated-declarations>"
    $<$<COMPILE_LANGUAGE:CXX>:-mcpu=cortex-m4>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-lto>
    $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m4>
    $<$<COMPILE_LANGUAGE:ASM>:-mthumb>
    $<$<COMPILE_LANGUAGE:ASM>:-mfpu=fpv4-sp-d16>
    $<$<COMPILE_LANGUAGE:ASM>:-mfloat-abi=softfp>
    "$<$<COMPILE_LANGUAGE:ASM>:SHELL:-x assembler-with-cpp>"
)

set(post_build_command ${POST_BUILD_EXE} postbuild "./Debugger-SiW917_station.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:Debugger-SiW917_station>")
set_property(TARGET slc PROPERTY C_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_STANDARD 17)
set_property(TARGET slc PROPERTY CXX_EXTENSIONS OFF)

target_link_options(slc INTERFACE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=softfp
    "-T${CMAKE_CURRENT_LIST_DIR}/../autogen/linkerfile_SoC.ld"
    --specs=nano.specs
    -Wl,-Map=$<TARGET_FILE_DIR:Debugger-SiW917_station>/Debugger-SiW917_station.map
    -Wl,--wrap=main
    "SHELL:-u _printf_float"
    -fno-lto
    -Wl,--gc-sections
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA