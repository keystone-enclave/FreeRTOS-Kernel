CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld
OBJCPY = riscv32-unknown-elf-objcopy

CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d
LDFLAGS = -static -nostdlib $(shell $(CC) --print-file-name=libgcc.a)

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./lib/rtos/include  -I./fs/include -I./lib/utils/elf/include -I./lib/utils/lib/include
CFLAGS += $(OPTION_FLAGS)

TARGET = FreeRTOS.elf

# FS_SRC = fs/src/diskio.c fs/src/ff.c fs/src/ffsystem.c

MALLOC_SRC = lib/utils/MemMang/heap_4.c

SRCS = lib/rtos/src/croutine.c lib/rtos/src/event_groups.c lib/rtos/src/list.c lib/rtos/src/queue.c lib/rtos/src/stream_buffer.c lib/rtos/src/tasks.c lib/rtos/src/timers.c lib/rtos/src/port.c lib/utils/lib/src/string.c lib/main/src/main.c lib/rtos/src/sbi.c lib/utils/lib/src/printf.c lib/rtos/src/syscall.c lib/utils/elf/src/elf32.c lib/utils/elf/src/elf.c
 
ASM_SRCS = firmware/portASM.S

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) 
	$(CC) -o $(TARGET) $(CFLAGS) $(ASM_SRCS) $(SRCS) $(FS_SRC) $(MALLOC_SRC) -T firmware/rtos.lds $(LDFLAGS)
clean:
	-rm $(TARGET)  
