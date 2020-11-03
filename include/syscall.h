#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "sbi.h"
#include "regs.h"

#define SYSCALL_SET_TIMER                   1000
#define SYSCALL_CONSOLE_PUTCHAR             1001
#define SYSCALL_CONSOLE_GETCHAR             1002
#define SYSCALL_YIELD                       1003
#define SYSCALL_TASK_DELETE                 1004

void handle_syscalls(struct encl_ctx *ctx);

void syscall_putchar(char character);

void syscall_yield();

void syscall_task_delete();


#endif /* syscall.h */