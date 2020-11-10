CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld
OBJCPY = riscv32-unknown-elf-objcopy

CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d
LDFLAGS = -static -nostdlib $(shell $(CC) --print-file-name=libgcc.a)

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./include  -I./fs/include
CFLAGS += $(OPTION_FLAGS)

TARGET = FreeRTOS.elf

# FS_SRC = fs/src/diskio.c fs/src/ff.c fs/src/ffsystem.c

MALLOC_SRC = MemMang/heap_4.c

SRCS = src/croutine.c src/event_groups.c src/list.c src/queue.c src/stream_buffer.c src/tasks.c src/timers.c src/port.c src/string.c src/main.c src/sbi.c src/printf.c src/syscall.c src/elf32.c src/elf.c
 
ASM_SRCS = src/portASM.S

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) 
	$(CC) -o $(TARGET) $(CFLAGS) $(ASM_SRCS) $(SRCS) $(FS_SRC) $(MALLOC_SRC) -T rtos.lds $(LDFLAGS)
clean:
	-rm $(TARGET)  
