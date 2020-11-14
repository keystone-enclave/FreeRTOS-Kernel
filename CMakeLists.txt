cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER "riscv32-unknown-linux-gnu-gcc")
set(CMAKE_C_LINK_EXECUTABLE "riscv32-unknown-linux-gnu-ld")
set(CMAKE_OBJCOPY "riscv32-unknown-elf-objcopy")
project(FreeRTOS.elf C ASM)
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

list(INSERT packages 0 ${rtos_src})

foreach(package ${packages})
    add_subdirectory(${package})
    include_directories(${INC_DIRS})
endforeach()


add_executable(FreeRTOS.elf ${main_src}/src/main.c)

target_link_libraries(FreeRTOS.elf rtos)
install(TARGETS FreeRTOS.elf DESTINATION bin)