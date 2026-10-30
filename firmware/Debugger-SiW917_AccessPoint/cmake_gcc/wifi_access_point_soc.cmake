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