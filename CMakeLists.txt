cmake_minimum_required(VERSION 3.5)

if (NOT TARGET_NAME)
    message(FATAL_ERROR "Missing required parameter TARGET_NAME")
endif()

if (NOT TARGET_VERSION)
    message(FATAL_ERROR "Missing required parameter TARGET_VERSION")
endif()

# Set PICO settings
set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk)
set(PICO_BOARD pico_w)

# Set general build settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set JERRYSCRIPT settings
set(JERRYSCRIPT_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/jerryscript)
set(JERRYSCRIPT_TOOLCHAIN ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript_toolchain.cmake)

set(JERRYSCRIPT_LIBS
  ${JERRYSCRIPT_SDK_PATH}/build/lib/libjerry-core.a
  ${JERRYSCRIPT_SDK_PATH}/build/lib/libjerry-ext.a
  ${JERRYSCRIPT_SDK_PATH}/build/lib/libjerry-port-default.a)

set(JERRYSCRIPT_INC
  ${JERRYSCRIPT_SDK_PATH}/jerry-core
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/api
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/debugger
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/ecma/base
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/ecma/builtin-objects
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/ecma/builtin-objects/typedarray
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/ecma/operations
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/include
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/jcontext
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/jmem
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/jrt
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/lit
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/parser/js
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/parser/regexp
  ${JERRYSCRIPT_SDK_PATH}/jerry-core/vm
  ${JERRYSCRIPT_SDK_PATH}/jerry-ext/arg
  ${JERRYSCRIPT_SDK_PATH}/jerry-ext/include
  ${JERRYSCRIPT_SDK_PATH}/jerry-libm)


set(JERRYSCRIPT_BUILD_ARGS
--toolchain=${JERRYSCRIPT_TOOLCHAIN}
  #--build-type=Debug
  --compile-flag="-DJERRY_NDEBUG=1 -DJERRY_LCACHE=0 -DJERRY_PROPRETY_HASHMAP=0"
  --lto=OFF
  --error-messages=ON
  --js-parser=ON
  --mem-heap=180
  --mem-stats=ON
  --snapshot-exec=ON
  --line-info=ON
  --vm-exec-stop=ON
  --profile=es.next #es2015-subset
  --jerry-cmdline=OFF
  #--logging=ON
  #--stack-limit=1024
  #--gc-limit=100000
  --cpointer-32bit=ON)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico-sdk-js CXX C ASM)

# Initialize PICO_SDK
pico_sdk_init()

# run jerryscript build
add_custom_command(OUTPUT ${JERRYSCRIPT_LIBS}
  WORKING_DIRECTORY ${JERRYSCRIPT_SDK_PATH}
  COMMAND python tools/build.py --clean ${JERRYSCRIPT_BUILD_ARGS})

# Set PICO-SDK-JS build settings
set(OUTPUT_TARGET ${TARGET_NAME}-${TARGET_VERSION})

set(SOURCES
  ${SOURCES} 
  ${CMAKE_CURRENT_LIST_DIR}/src/main.c)

set(INCLUDES
  ${INCLUDES}
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${JERRYSCRIPT_INC})

add_compile_definitions(TARGET_NAME="${TARGET_NAME}")
add_compile_definitions(TARGET_VERSION="${TARGET_VERSION}")
include_directories(${OUTPUT_TARGET} ${INCLUDES})
add_executable(${OUTPUT_TARGET} ${SOURCES} ${JERRYSCRIPT_LIBS})

target_link_libraries(${OUTPUT_TARGET} pico_stdlib ${JERRYSCRIPT_LIBS})

# Enable USB output, disable UART output
pico_enable_stdio_usb(${OUTPUT_TARGET} 1)
pico_enable_stdio_uart(${OUTPUT_TARGET} 0)
pico_add_extra_outputs(${OUTPUT_TARGET})
