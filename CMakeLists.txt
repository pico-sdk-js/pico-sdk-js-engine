cmake_minimum_required(VERSION 3.5)

if (NOT TARGET_NAME)
    message(FATAL_ERROR "Missing required parameter TARGET_NAME")
endif()

if (NOT TARGET_VERSION)
    message(FATAL_ERROR "Missing required parameter TARGET_VERSION")
endif()

set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico-sdk-js CXX C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Default to Pico_W board (consider future support for base Pico board)
set(PICO_BOARD pico_w)

pico_sdk_init()

set(SOURCES
  ${SOURCES} 
  ${CMAKE_CURRENT_LIST_DIR}/src/main.c)

set(OUTPUT_TARGET ${TARGET_NAME}-${TARGET_VERSION})
add_executable(${OUTPUT_TARGET} ${SOURCES})
target_link_libraries(${OUTPUT_TARGET} pico_stdlib)

# Enable USB output, disable UART output
pico_enable_stdio_usb(${OUTPUT_TARGET} 1)
pico_enable_stdio_uart(${OUTPUT_TARGET} 0)

pico_add_extra_outputs(${OUTPUT_TARGET})
