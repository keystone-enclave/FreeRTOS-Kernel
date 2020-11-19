CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld
OBJCPY = riscv32-unknown-elf-objcopy
OBJDUMP = riscv32-unknown-elf-objdump

CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d
LDFLAGS = -static -nostdlib

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./lib/rtos/include  -I./fs/include -I./lib/utils/elf/include -I./lib/utils/lib/include
CFLAGS += $(OPTION_FLAGS)

TARGET = freeRTOS

MALLOC_SRC = lib/utils/MemMang/src/heap_4.c

SRCS = lib/rtos/src/croutine.c lib/rtos/src/event_groups.c lib/rtos/src/list.c lib/rtos/src/queue.c lib/rtos/src/stream_buffer.c lib/rtos/src/tasks.c lib/rtos/src/timers.c lib/rtos/src/port.c lib/utils/lib/src/string.c main/src/main.c lib/rtos/src/sbi.c lib/utils/lib/src/printf.c lib/utils/lib/src/stdio.c lib/rtos/src/syscall.c lib/rtos/src/enclave.c lib/utils/elf/src/elf32.c lib/utils/elf/src/elf.c

CFLAGS += -I./lib/utils/FreeRTOS-Plus-CLI/include
SRCS += lib/utils/FreeRTOS-Plus-CLI/src/FreeRTOS_CLI.c lib/utils/FreeRTOS-Plus-CLI/src/commands.c lib/utils/FreeRTOS-Plus-CLI/src/console.c

CFLAGS += -I./lib/utils/FreeRTOS-Plus-IO
SRCS += lib/utils/FreeRTOS-Plus-IO/FreeRTOS_IO.c
 
ASM_SRCS = lib/rtos/firmware/portASM.S

TESTS = tests/fibonacci/fibonacci.S tests/attest/attest.S

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) tests
	$(CC) -o $(TARGET) $(CFLAGS) $(ASM_SRCS) $(SRCS) $(TESTS) $(MALLOC_SRC) -T lib/rtos/firmware/rtos.lds $(LDFLAGS)
	$(OBJDUMP) -D $(TARGET) > elf.txt

tests:
	cd tests/fibonacci && $(MAKE)
	cd tests/attest && $(MAKE)

clean:
	-rm $(TARGET)  
	