project(rtos C ASM)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T firmware/rtos.lds")

set(CFLAGS      "-Wall -O3 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d")
set(LDFLAGS     "-static -nostdlib $(shell $(CC) --print-file-name=libgcc.a)")

file(GLOB SOURCE_FILES "src/*.c")

set(INCLUDE_DIRS include)

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    set(INC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIRS} PARENT_SCOPE)
endif()

set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

include_directories(${INCLUDE_DIRS})

add_library(${PROJECT_NAME} firmware/portASM.S ${SOURCE_FILES})

target_compile_definitions(${PROJECT_NAME} PUBLIC -DportasmHANDLE_INTERRUPT=interrupt_handler)

install(TARGETS ${PROJECT_NAME} DESTINATION ${out_dir}/lib)
install(DIRECTORY ${INCLUDE_DIRS} DESTINATION ${out_dir}/include)