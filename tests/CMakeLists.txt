if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    include(../macros.cmake)
    use_riscv_toolchain(32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -nostdlib")
endif()

cmake_minimum_required(VERSION 3.6)
project(tests C ASM)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(lib/include)

file(GLOB LIB_SOURCE_FILES "lib/src/*")



add_executable(rtos_fibonacci fibonacci/fibonacci.c ${LIB_SOURCE_FILES})
set_target_properties(rtos_fibonacci PROPERTIES LINK_FLAGS "-static -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")
set_target_properties(rtos_fibonacci PROPERTIES COMPILE_FLAGS "-Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
install(TARGETS rtos_fibonacci DESTINATION ${EXECUTABLE_OUTPUT_PATH})

add_executable(rtos_attest attest/attest.c ${LIB_SOURCE_FILES})
set_target_properties(rtos_attest PROPERTIES LINK_FLAGS "-static -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")
set_target_properties(rtos_attest PROPERTIES COMPILE_FLAGS "-Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
install(TARGETS rtos_attest DESTINATION ${EXECUTABLE_OUTPUT_PATH})

add_executable(rtos_agent agent/agent.c ${LIB_SOURCE_FILES})
set_target_properties(rtos_agent PROPERTIES LINK_FLAGS "-static -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")
set_target_properties(rtos_agent PROPERTIES COMPILE_FLAGS "-Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
install(TARGETS rtos_agent DESTINATION ${EXECUTABLE_OUTPUT_PATH})

add_executable(rtos_simulator simulator/simulator.c ${LIB_SOURCE_FILES})
set_target_properties(rtos_simulator PROPERTIES LINK_FLAGS "-static -nostdlib -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")
set_target_properties(rtos_simulator PROPERTIES COMPILE_FLAGS "-Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss")
install(TARGETS rtos_simulator DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set(LINK_FILES_PATH ${EXECUTABLE_OUTPUT_PATH}/tests/src)
file(MAKE_DIRECTORY ${LINK_FILES_PATH})

set(FILES ${LINK_FILES_PATH}/rtos_fibonacci.S ${LINK_FILES_PATH}/rtos_attest.S ${LINK_FILES_PATH}/rtos_agent.S ${LINK_FILES_PATH}/rtos_simulator.S)

# execute_process(
#     COMMAND touch ${FILES}
# )

file (WRITE ${LINK_FILES_PATH}/rtos_fibonacci.S ".section .fibonacci\n    .incbin \"${EXECUTABLE_OUTPUT_PATH}/rtos_fibonacci\" ")
file (WRITE ${LINK_FILES_PATH}/rtos_attest.S ".section .attest\n    .incbin \"${EXECUTABLE_OUTPUT_PATH}/rtos_attest\" ")
file (WRITE ${LINK_FILES_PATH}/rtos_agent.S ".section .agent\n    .incbin \"${EXECUTABLE_OUTPUT_PATH}/rtos_agent\" ")
file (WRITE ${LINK_FILES_PATH}/rtos_simulator.S ".section .simulator\n    .incbin \"${EXECUTABLE_OUTPUT_PATH}/rtos_simulator\" ")


# These are required variables
set(TEST_FILES ${FILES} PARENT_SCOPE)
set(TEST_NAMES rtos_fibonacci rtos_attest rtos_agent rtos_simulator PARENT_SCOPE)