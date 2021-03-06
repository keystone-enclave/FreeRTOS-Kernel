#ifndef __SBI_H_
#define __SBI_H_

#include <stdint.h>

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2

#define SBI_SM_CREATE_ENCLAVE    101
#define SBI_SM_DESTROY_ENCLAVE   102
#define SBI_SM_ATTEST_ENCLAVE    103
#define SBI_SM_GET_SEALING_KEY   104
#define SBI_SM_RUN_ENCLAVE       105
#define SBI_SM_STOP_ENCLAVE      106
#define SBI_SM_RESUME_ENCLAVE    107
#define SBI_SM_RANDOM            108

#define SBI_SEND_TASK      204
#define SBI_RECV_TASK      205

#define SBI_SM_EXIT_ENCLAVE     1101
#define SBI_SM_CALL_PLUGIN      1000
#define SBI_SM_NOT_IMPLEMENTED  1111

#define SBI_CALL(___which, ___arg0, ___arg1, ___arg2, ___arg3)            \
  ({                                                             \
    register uintptr_t a0 __asm__("a0") = (uintptr_t)(___arg0);  \
    register uintptr_t a1 __asm__("a1") = (uintptr_t)(___arg1);  \
    register uintptr_t a2 __asm__("a2") = (uintptr_t)(___arg2);  \
    register uintptr_t a3 __asm__("a3") = (uintptr_t)(___arg3);  \
    register uintptr_t a7 __asm__("a7") = (uintptr_t)(___which); \
    __asm__ volatile("ecall"                                     \
                     : "+r"(a0)                                  \
                     : "r"(a1), "r"(a2), "r"(a3), "r"(a7)                 \
                     : "memory");                                \
    a0;                                                          \
  })

/* Lazy implementations until SBI is finalized */
#define SBI_CALL_0(___which) SBI_CALL(___which, 0, 0, 0, 0)
#define SBI_CALL_1(___which, ___arg0) SBI_CALL(___which, ___arg0, 0, 0, 0)
#define SBI_CALL_2(___which, ___arg0, ___arg1) \
  SBI_CALL(___which, ___arg0, ___arg1, 0, 0)
#define SBI_CALL_3(___which, ___arg0, ___arg1, ___arg2) \
  SBI_CALL(___which, ___arg0, ___arg1, ___arg2, 0)
#define SBI_CALL_4(___which, ___arg0, ___arg1, ___arg2, ___arg3) \
  SBI_CALL(___which, ___arg0, ___arg1, ___arg2, ___arg3)

int
sbi_putchar(int c);

int
sbi_getchar();

void
sbi_set_timer(uint64_t stime_value); 

void
sbi_switch_task();

void
sbi_enable_interrupts();

void
sbi_disable_interrupts();

void handle_timer_interrupt();

void ENABLE_INTERRUPTS(void);
void DISABLE_INTERRUPTS(void); 

int sbi_recv(int task_id, void *msg, int size, uintptr_t yield);
int sbi_send(int task_id, void *msg, int size, uintptr_t yield);

#endif
