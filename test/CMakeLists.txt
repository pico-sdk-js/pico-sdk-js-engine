# set minimum cmake version
cmake_minimum_required(VERSION 3.14)

# project name and language
project(${TARGET_NAME}_TEST CXX C ASM)

# require C++14
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

set(TEST_OUTPUT_TARGET ${TARGET_NAME}_TEST)

message(${INCLUDES})

# testing binary
include_directories(${TEST_OUTPUT_TARGET} ${INCLUDES})
add_executable(${TEST_OUTPUT_TARGET} ${TEST_SOURCES})
target_link_libraries(${TEST_OUTPUT_TARGET} PRIVATE ${LIBRARIES} ${OUTPUT_LIB})

enable_testing()
add_test(NAME my_add_test COMMAND ${TEST_OUTPUT_TARGET})