#ifndef __EAPP_H__
#define __EAPP_H__

#include <stdint.h>

#define SBI_CONSOLE_PUTCHAR 1

#define RET_EXIT 0 
#define RET_YIELD 1

#define SBI_SWITCH_TASK    201
#define SBI_ATTEST_TASK    203
#define SBI_SEND_TASK      204
#define SBI_RECV_TASK      205

#define SBI_SM_UID               109
#define SBI_SM_MAILBOX_SEND      110
#define SBI_SM_MAILBOX_RECV      111

#define SBI_CALL(___which, ___arg0, ___arg1, ___arg2)            \
  ({                                                             \
    register uintptr_t a0 __asm__("a0") = (uintptr_t)(___arg0);  \
    register uintptr_t a1 __asm__("a1") = (uintptr_t)(___arg1);  \
    register uintptr_t a2 __asm__("a2") = (uintptr_t)(___arg2);  \
    register uintptr_t a7 __asm__("a7") = (uintptr_t)(___which); \
    __asm__ volatile("ecall"                                     \
                     : "+r"(a0)                                  \
                     : "r"(a1), "r"(a2), "r"(a7)                 \
                     : "memory");                                \
    a0;                                                          \
  })

/* Lazy implementations until SBI is finalized */
#define SBI_CALL_0(___which) SBI_CALL(___which, 0, 0, 0)
#define SBI_CALL_1(___which, ___arg0) SBI_CALL(___which, ___arg0, 0, 0)
#define SBI_CALL_2(___which, ___arg0, ___arg1) \
  SBI_CALL(___which, ___arg0, ___arg1, 0)
#define SBI_CALL_3(___which, ___arg0, ___arg1, ___arg2) \
  SBI_CALL(___which, ___arg0, ___arg1, ___arg2)

#define EAPP_ENTRY __attribute__((__section__(".text._start")))

void
sbi_putchar(char character);

void syscall_task_yield();

void syscall_task_return();


int sbi_recv(int task_id, void *msg, int size);
int sbi_send(int task_id, void *msg, int size);

#endif