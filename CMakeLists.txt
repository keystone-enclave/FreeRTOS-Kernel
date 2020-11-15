include(macros.cmake)

cmake_minimum_required(VERSION 3.5)
project(FreeRTOS.elf C ASM)
use_riscv_toolchain(32)
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
execute_process(COMMAND ${CMAKE_C_COMPILER} "--print-file-name=libgcc.a" OUTPUT_VARIABLE LIBGCC)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBGCC}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DportasmHANDLE_INTERRUPT=interrupt_handler")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

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

set(CFLAGS      "-Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d")
execute_process(COMMAND ${CMAKE_C_COMPILER} --print-file-name=libgcc.a OUTPUT_VARIABLE LIBGCC)
set(LDFLAGS     "-static -nostdlib ${LIBGCC}")
set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} ${CFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

include_directories(${rtos_src}/include)
include_directories(${main_src}/include)

SUBDIRLIST(SUBDIRS ${utils_dir_src})
message("Checking for packages...")
SET(packages "")
SET(package_names "")
FOREACH(package ${SUBDIRS})
  set(pkg ${utils_dir_src}/${package})
  message("Adding ${pkg}?")
  if(EXISTS "${pkg}/CMakeLists.txt")
    message("Added!")
    LIST(APPEND packages ${pkg})
    LIST(APPEND package_names ${package})
  else()
    message("Not a package!")
  endif()
ENDFOREACH()

foreach(package ${packages})
    add_subdirectory(${package})
    include_directories(${INC_DIRS})
endforeach()

# Build RTOS
file(GLOB RTOS_SOURCE_FILES "${rtos_src}/src/*.c")
file(GLOB MAIN_SOURCE_FILES "${main_src}/src/*.c")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${rtos_src}/firmware/rtos.lds")


add_executable(FreeRTOS.elf ${rtos_src}/firmware/portASM.S ${RTOS_SOURCE_FILES} ${MAIN_SOURCE_FILES} ${packages})

target_compile_definitions(FreeRTOS.elf PUBLIC -DportasmHANDLE_INTERRUPT=interrupt_handler)

install(TARGETS FreeRTOS.elf DESTINATION bin)

# install(TARGETS ${PROJECT_NAME} DESTINATION ${out_dir}/lib)
# install(DIRECTORY ${INCLUDE_DIRS} DESTINATION ${out_dir}/include)