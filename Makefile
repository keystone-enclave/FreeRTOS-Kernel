CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld


CFLAGS = -Wall -O3 -Werror -fPIC -fno-builtin -std=c11 -g -march=rv32imafd -mabi=ilp32d
LDFLAGS = -static -nostdlib

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./include 
CFLAGS += $(OPTION_FLAGS)

TARGET = FreeRTOS.elf

SRCS = croutine.c event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c port.c MemMang/heap_5.c string.c main.c 
 
ASM_SRCS = portASM.S


all: $(TARGET)

$(TARGET): 
	$(CC) -o $(TARGET) $(CFLAGS) $(ASM_SRCS) $(SRCS) -T rtos.lds $(LDFLAGS)

clean:
	rm $(TARGET)  
