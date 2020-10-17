CC = riscv32-unknown-linux-gnu-gcc
LINK = riscv32-unknown-linux-gnu-ld


CFLAGS = -Wall -Werror -fPIC -fno-builtin -std=c11 -g 
LDFLAGS = -static

OPTION_FLAGS = -DportasmHANDLE_INTERRUPT=interrupt_handler

CFLAGS += -I./include 
CFLAGS += $(OPTION_FLAGS)

TARGET = FreeRTOS.elf

SRCS = croutine.c event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c port.c MemMang/heap_5.c main.c 
 
ASM_SRCS = portASM.S


all: $(TARGET)

$(TARGET): 
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) $(ASM_SRCS) -T flash.lds $(LDFLAGS)

clean:
	rm $(TARGET)  
