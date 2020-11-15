CC = riscv32-unknown-linux-gnu-gcc
OBJDUMP = riscv32-unknown-linux-gnu-objdump
CFLAGS = -Wall -O0 -Werror -fPIC -fno-builtin -g -std=c11 -fcommon -fno-zero-initialized-in-bss
LDFLAGS = -static -nostdlib 

LIBDIR = ../lib

TARGET = attest.bin

SRCS = attest.c $(LIBDIR)/src/eapp.S $(LIBDIR)/src/printf.c $(LIBDIR)/src/string.c $(LIBDIR)/src/eapp_utils.c 

CFLAGS += -I.. -I$(LIBDIR)/include

.PHONY: clean

all: $(TARGET)

$(TARGET): clean $(ASM_SRCS) $(SRCS) 
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) -T ../app.lds $(LDFLAGS)
	$(OBJDUMP) -D $(TARGET) > app.txt

clean:
	-rm $(TARGET)  
