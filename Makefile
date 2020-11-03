CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld
OBJCPY = riscv32-unknown-elf-objcopy

CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d
LDFLAGS = -static -nostdlib $(shell $(CC) --print-file-name=libgcc.a)

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./include 
CFLAGS += $(OPTION_FLAGS)

TARGET = FreeRTOS.elf

SRCS = croutine.c event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c port.c MemMang/heap_4.c string.c main.c sbi.c printf.c syscall.c 
 
ASM_SRCS = portASM.S

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) 
	$(CC) -o $(TARGET) $(CFLAGS) $(ASM_SRCS) $(SRCS) -T rtos.lds $(LDFLAGS)
clean:
	-rm $(TARGET)  
