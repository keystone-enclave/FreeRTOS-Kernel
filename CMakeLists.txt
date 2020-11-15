if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
  include(macros.cmake)
  use_riscv_toolchain(32)
# else()
#   set(CMAKE_C_COMPILER "riscv32-unknown-linux-gnu-gcc")
#   set(CMAKE_C_LINK_EXECUTABLE "riscv32-unknown-linux-gnu-ld")
#   set(CMAKE_OBJCOPY "riscv32-unknown-elf-objcopy")
endif()

cmake_minimum_required(VERSION 3.5)
project(freeRTOS C ASM)
set(CMAKE_VERBOSE_MAKEFILE ON)

###############################################################################
## MACROS
###############################################################################

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

###############################################################################
## CONFIGURATION/VARIABLES
###############################################################################

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -nostdlib")
# execute_process(COMMAND ${CMAKE_C_COMPILER} "--print-file-name=libgcc.a" OUTPUT_VARIABLE LIBGCC)
# string(STRIP ${LIBGCC} LIBGCC)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBGCC}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DportasmHANDLE_INTERRUPT=interrupt_handler")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(lib_src ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(main_src main)

set(rtos_src ${lib_src}/rtos)
set(utils_dir_src ${lib_src}/utils)

set(ASM_SRCS "${ASM_SRCS}")
set(SRCS "${SRCS}")


###############################################################################
## Build System
###############################################################################

# add_subdirectory(${firmware_src})
# add_subdirectory(${main_src})
# add_subdirectory(${rtos_src})

# add_subdirectory(${utils_dir_src}/elf)
# add_subdirectory(${utils_dir_src}/MemMang)
# add_subdirectory(${utils_dir_src}/lib)

# message(STATUS ${ASM_SRCS})
# message(STATUS ${SRCS})

include_directories(${rtos_src}/include)
include_directories(${main_src}/include)

# SUBDIRLIST(SUBDIRS ${utils_dir_src})
# message("Checking for packages...")
# SET(packages "")
# SET(package_names "")
# FOREACH(package ${SUBDIRS})
#   set(pkg ${utils_dir_src}/${package})
#   message("Adding ${pkg}?")
#   if(EXISTS "${pkg}/CMakeLists.txt")
#     message("Added!")
#     LIST(APPEND packages ${pkg})
#     LIST(APPEND package_names ${package})
#   else()
#     message("Not a package!")
#   endif()
# ENDFOREACH()

# foreach(package ${packages})
#     add_subdirectory(${package})
#     message("Adding package ${package}")
#     include_directories(${INC_DIRS})
# endforeach()

# Build RTOS
file(GLOB RTOS_SOURCE_FILES "${rtos_src}/src/*.c")
file(GLOB MAIN_SOURCE_FILES "${main_src}/src/*.c")

file(GLOB ELF_SOURCE_FILES "${utils_dir_src}/elf/src/*.c")
file(GLOB LIB_SOURCE_FILES "${utils_dir_src}/lib/src/*.c")
file(GLOB MEMMANG_SOURCE_FILES "${utils_dir_src}/MemMang/src/heap_4.c")
include_directories(${utils_dir_src}/elf/include)
include_directories(${utils_dir_src}/lib/include)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${rtos_src}/firmware/rtos.lds")

set_source_files_properties(${rtos_src}/firmware/portASM.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")

add_subdirectory(tests)

add_executable(freeRTOS ${rtos_src}/firmware/portASM.S ${RTOS_SOURCE_FILES} ${ELF_SOURCE_FILES} ${LIB_SOURCE_FILES} ${MEMMANG_SOURCE_FILES} ${MAIN_SOURCE_FILES})
target_compile_definitions(freeRTOS PUBLIC -DportasmHANDLE_INTERRUPT=interrupt_handler)

set_property(TARGET freeRTOS PROPERTY LINK_FLAGS ${LIBGCC})

target_link_libraries(freeRTOS fib att)

install(TARGETS freeRTOS DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# install(TARGETS ${PROJECT_NAME} DESTINATION ${out_dir}/lib)
# install(DIRECTORY ${INCLUDE_DIRS} DESTINATION ${out_dir}/include)