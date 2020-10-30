#ifndef __SBI_H_
#define __SBI_H_

#include <stdint.h>

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2

#define SBI_ENABLE_INTERRUPT 109
#define SBI_DISABLE_INTERRUPT 110 

void
sbi_putchar(char c);

uintptr_t ENABLE_INTERRUPTS(void);
uintptr_t DISABLE_INTERRUPTS(void); 

#endif
