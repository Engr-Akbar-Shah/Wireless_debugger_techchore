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

set(post_build_command ${POST_BUILD_EXE} postbuild "./Debugger-SiW917_AccessPoint.slpb" --parameter build_dir:"$<TARGET_FILE_DIR:Debugger-SiW917_AccessPoint>")
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
    -Wl,-Map=$<TARGET_FILE_DIR:Debugger-SiW917_AccessPoint>/Debugger-SiW917_AccessPoint.map
    -Wl,--wrap=main
    "SHELL:-u _printf_float"
    -fno-lto
    -Wl,--gc-sections
)

# BEGIN_SIMPLICITY_STUDIO_METADATA=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END_SIMPLICITY_STUDIO_METADATA