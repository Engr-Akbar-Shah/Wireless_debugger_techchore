# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	Debugger-SiW917_AccessPoint
	VERSION 1.0
	LANGUAGES C CXX ASM
)

add_compile_definitions(
    SLI_SI91X_MCU_INTR_BASED_RX_ON_UART=1
)

# Include the definition of the slc_Debugger-SiW917_AccessPoint target,
# which contains the content of the SLC project
include(Debugger-SiW917_AccessPoint.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/../../bridge_sdk_fixes.cmake")

# Keep ULP UART DMA reception, but use the SDK FIFO interrupt transmitter.
# The coupled DMA option normally enables both; sustained tests isolated extra
# bytes to ULP DMA TX. Generate this one-line adaptation without editing SDK files.
set(bridge_uart_vendor "${CMAKE_CURRENT_LIST_DIR}/../wiseconnect3_sdk_4.0.2/components/device/silabs/si91x/mcu/drivers/cmsis_driver/USART.c")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${bridge_uart_vendor}")
get_target_property(bridge_sources slc SOURCES)
list(FILTER bridge_sources EXCLUDE REGEX "/cmsis_driver/USART[.]c$")
set_property(TARGET slc PROPERTY SOURCES "${bridge_sources}")
file(READ "${bridge_uart_vendor}" bridge_uart_source)
string(FIND "${bridge_uart_source}" "#define RTE_ULPUART_CHNL_UDMA_TX_EN 1" bridge_uart_patch_position)
if(bridge_uart_patch_position EQUAL -1)
  message(FATAL_ERROR "Vendor UART DMA TX switch changed; review the RX-only adaptation")
endif()
string(REPLACE "#define RTE_ULPUART_CHNL_UDMA_TX_EN 1" "#define RTE_ULPUART_CHNL_UDMA_TX_EN 0 /* Bridge: FIFO IRQ TX, DMA RX */" bridge_uart_source "${bridge_uart_source}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/bridge_USART.c" "${bridge_uart_source}")
target_sources(slc PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/bridge_USART.c")


add_executable(Debugger-SiW917_AccessPoint
    # Add additional sources here
)

target_include_directories(Debugger-SiW917_AccessPoint PUBLIC
    # Add additional include paths here
)

target_compile_definitions(Debugger-SiW917_AccessPoint PUBLIC
    # Add additional macros here
)

target_compile_options(Debugger-SiW917_AccessPoint PUBLIC
    # Set additional compiler flags here
)

target_link_options(Debugger-SiW917_AccessPoint PUBLIC
    "-Wl,--wrap=_write"
)

# Link with the content defined in the SLC project
target_link_libraries(Debugger-SiW917_AccessPoint PRIVATE
    slc
)

# Include managed project content if available
include(Debugger-SiW917_AccessPoint_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(Debugger-SiW917_AccessPoint PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET Debugger-SiW917_AccessPoint
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:Debugger-SiW917_AccessPoint>" "$<TARGET_FILE_DIR:Debugger-SiW917_AccessPoint>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_AccessPoint>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:Debugger-SiW917_AccessPoint>" "$<TARGET_FILE_DIR:Debugger-SiW917_AccessPoint>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_AccessPoint>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:Debugger-SiW917_AccessPoint>" "$<TARGET_FILE_DIR:Debugger-SiW917_AccessPoint>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_AccessPoint>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET Debugger-SiW917_AccessPoint
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()

# Track C allocation failure without changing allocation semantics.
target_link_options(Debugger-SiW917_AccessPoint PUBLIC "-Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc")

target_sources(Debugger-SiW917_AccessPoint PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../host_uart.c")


# Stack audit for the application call chains changed by the transport correction.
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/../app.c" "${CMAKE_CURRENT_LIST_DIR}/../host_uart.c" PROPERTIES COMPILE_OPTIONS "-fstack-usage")
