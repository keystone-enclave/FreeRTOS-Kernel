if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    include(../macros.cmake)
    use_riscv_toolchain(32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -nostdlib")
endif()

cmake_minimum_required(VERSION 3.5)
project(tests C ASM)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(lib/include)

file(GLOB LIB_SOURCE_FILES "lib/src/*")


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")

add_executable(fib fibonacci/fibonacci.c ${LIB_SOURCE_FILES})
install(TARGETS fib DESTINATION ${EXECUTABLE_OUTPUT_PATH})

add_executable(att attest/attest.c ${LIB_SOURCE_FILES})
install(TARGETS att DESTINATION ${EXECUTABLE_OUTPUT_PATH})