CC = riscv32-unknown-linux-gnu-gcc
OBJDUMP = riscv32-unknown-linux-gnu-objdump
CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss
LDFLAGS = -static -nostdlib

TARGET = fibonacci.bin

SRCS = fibonacci.c ../eapp.S

CFLAGS += -I..

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) 
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) -T ../app.lds $(LDFLAGS)
	$(OBJDUMP) -D $(TARGET) > app.txt

clean:
	-rm $(TARGET)  

test:
	$(CC) -o test.o fibonacci.S -nostdlib -T test.lds
	$(OBJDUMP) -D test.o > test.txt
