# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	Debugger-SiW917_station
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_Debugger-SiW917_station target,
# which contains the content of the SLC project
include(Debugger-SiW917_station.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/../../bridge_sdk_fixes.cmake")

add_executable(Debugger-SiW917_station
    # Add additional sources here
)

target_include_directories(Debugger-SiW917_station PUBLIC
    # Add additional include paths here
)

target_compile_definitions(Debugger-SiW917_station PUBLIC
    # Add additional macros here
)

target_compile_options(Debugger-SiW917_station PUBLIC
    # Set additional compiler flags here
)

target_link_options(Debugger-SiW917_station PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(Debugger-SiW917_station PRIVATE
    slc
)

# Include managed project content if available
include(Debugger-SiW917_station_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# VS Code's managed file or regenerated SLC content may already register this
# application source. Add a fallback only when neither target owns it, so the
# standalone workflow and the VS Code build both compile it exactly once.
get_filename_component(target_uart_source "../target_uart.c" ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(target_uart_registered FALSE)
foreach(uart_target IN ITEMS slc Debugger-SiW917_station)
    get_target_property(uart_sources ${uart_target} SOURCES)
    foreach(uart_source IN LISTS uart_sources)
        get_filename_component(uart_source_absolute "${uart_source}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(uart_source_absolute STREQUAL target_uart_source)
            set(target_uart_registered TRUE)
        endif()
    endforeach()
endforeach()
if(NOT target_uart_registered)
    target_sources(Debugger-SiW917_station PRIVATE "${target_uart_source}")
endif()

# Force the gcc linker command
set_target_properties(Debugger-SiW917_station PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET Debugger-SiW917_station
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:Debugger-SiW917_station>" "$<TARGET_FILE_DIR:Debugger-SiW917_station>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_station>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:Debugger-SiW917_station>" "$<TARGET_FILE_DIR:Debugger-SiW917_station>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_station>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:Debugger-SiW917_station>" "$<TARGET_FILE_DIR:Debugger-SiW917_station>/$<TARGET_FILE_BASE_NAME:Debugger-SiW917_station>.bin" 
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET Debugger-SiW917_station
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()

# Track C allocation failure without changing allocation semantics.
target_link_options(Debugger-SiW917_station PUBLIC "-Wl,--wrap=malloc,--wrap=calloc,--wrap=realloc")


# Stack audit for the application call chains changed by the transport correction.
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/../app.c" PROPERTIES COMPILE_OPTIONS "-fstack-usage")
