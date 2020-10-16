CC = riscv32-unknown-linux-gnu-gcc

CFLAGS += -I./include 
PORT_SRC = RISCV/

SRCS = croutine.c event_groups.c list.c queue.c stream_buffer.c tasks.c timers.c RISCV/portASM.S RISCV/port.c 



 
