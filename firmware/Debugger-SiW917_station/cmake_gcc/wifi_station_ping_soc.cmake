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