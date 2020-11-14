
#define SBI_CONSOLE_PUTCHAR 1
#define RET_EXIT 0 
#define RET_YIELD 1

#define SBI_SWITCH_TASK    201

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

typedef unsigned long uintptr_t;

void
syscall_putchar(char character) {
  SBI_CALL_1(SBI_CONSOLE_PUTCHAR, character);
}

/* 
    Yields the task back to the scheduler.
    MUST be called in a task context.
*/
void syscall_task_yield(){
    SBI_CALL_2(SBI_SWITCH_TASK, 0, RET_YIELD);
}


/* 
    Returns the task back to the scheduler.
    Signals the scheduler to self delete the task. 
    Will NOT return
*/
void syscall_task_return(){
    SBI_CALL_2(SBI_SWITCH_TASK, 0, RET_EXIT);
}
