cmake_minimum_required(VERSION 3.5)

if (NOT TARGET_NAME)
    message(FATAL_ERROR "Missing required parameter TARGET_NAME")
else()
  message("TARGET_NAME: " ${TARGET_NAME})
endif()

if (NOT TARGET_VERSION)
    message(FATAL_ERROR "Missing required parameter TARGET_VERSION")
else()
  message("TARGET_VERSION: " ${TARGET_VERSION})
endif()

if (NOT TARGET_OS)
  message(FATAL_ERROR "Missing required parameter TARGET_OS")
else()
  message("TARGET_OS: " ${TARGET_OS})
endif()

set(VALID_TARGET_OS "linux" "rp2xxx")

if (NOT TARGET_OS IN_LIST VALID_TARGET_OS)
  message(FATAL_ERROR "TARGET_OS must be either 'linux' or 'rp2xxx'")
endif()

set(CMAKE_BUILD_TYPE Debug)

# Set general build settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set JERRYSCRIPT settings
set(JERRYSCRIPT_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/jerryscript)

if (TARGET_OS STREQUAL "rp2xxx")
  set(JERRYSCRIPT_TOOLCHAIN ${CMAKE_CURRENT_SOURCE_DIR}/jerryscript_toolchain.cmake)
elseif(TARGET_OS STREQUAL "linux")
  set(JERRYSCRIPT_TOOLCHAIN ${JERRYSCRIPT_SDK_PATH}/cmake/toolchain_linux_i686.cmake)
endif()

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

# Setting UTHash
set(UTHASH_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/uthash)

set(UTHASH_INC
  ${UTHASH_SDK_PATH}/include)

# See https://jerryscript.net/configuration/ for additional options
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

if (TARGET_OS STREQUAL "rp2xxx")
  # Set PICO settings
  set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk)
  set(PICO_BOARD pico_w)

  include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif()

project(pico-sdk-js CXX C ASM)

if (TARGET_OS STREQUAL "rp2xxx")
  # Initialize PICO_SDK
  pico_sdk_init()
endif()

# run jerryscript build
add_custom_command(OUTPUT ${JERRYSCRIPT_LIBS}
  WORKING_DIRECTORY ${JERRYSCRIPT_SDK_PATH}
  COMMAND python tools/build.py --clean ${JERRYSCRIPT_BUILD_ARGS})

# Set PICO-SDK-JS build settings
set(OUTPUT_TARGET ${TARGET_NAME}-${TARGET_VERSION})

set(SOURCES
  ${SOURCES} 
  ${CMAKE_CURRENT_LIST_DIR}/src/io.c
  ${CMAKE_CURRENT_LIST_DIR}/src/main.c
  ${CMAKE_CURRENT_LIST_DIR}/src/repl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/os/${TARGET_OS}/os.c)

set(INCLUDES
  ${INCLUDES}
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${JERRYSCRIPT_INC}
  ${UTHASH_INC})

set(LIBRARIES
  ${LIBRARIES}
  ${JERRYSCRIPT_LIBS})

if (TARGET_OS STREQUAL "rp2xxx")
  set(LIBRARIES
    ${LIBRARIES}
    pico_stdlib)
elseif (TARGET_OS STREQUAL "linux")
  set(LIBRARIES
    ${LIBRARIES}
    c m)
endif()

add_compile_definitions(TARGET_NAME="${TARGET_NAME}")
add_compile_definitions(TARGET_VERSION="${TARGET_VERSION}")

include_directories(${OUTPUT_TARGET} ${INCLUDES})
add_executable(${OUTPUT_TARGET} ${SOURCES} ${JERRYSCRIPT_LIBS})
target_link_libraries(${OUTPUT_TARGET} ${LIBRARIES})

if (TARGET_OS STREQUAL "rp2xxx")
  # Enable USB output, disable UART output
  pico_enable_stdio_usb(${OUTPUT_TARGET} 1)
  pico_enable_stdio_uart(${OUTPUT_TARGET} 0)
  pico_add_extra_outputs(${OUTPUT_TARGET})
endif()