#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "sbi.h"
#include "regs.h"
#include <stddef.h>
#include <stdint.h>

#define SBI_SHUTDOWN 8

#define SBI_ENABLE_INTERRUPT     200
#define SBI_SWITCH_TASK          201
#define SBI_REGISTER_TASK        202


#define RET_EXIT 0 
#define RET_YIELD 1
#define RET_TIMER 2
#define RET_RECV_WAIT 3 

struct task_ctx {
	struct regs regs;
	uintptr_t valid; 
};

struct register_sbi_arg {
    uintptr_t pc;
	uintptr_t sp; 
	uintptr_t arg; 
	uintptr_t stack_size; 
	uintptr_t base;
	uintptr_t size;  
	uintptr_t enclave; 
}; 

void handle_syscalls(struct task_ctx *ctx);

int syscall_putchar(int character);

int syscall_getchar();

void syscall_task_yield();

uintptr_t syscall_switch_task(uintptr_t task_id, uintptr_t ret_type);

uintptr_t syscall_register_task (struct register_sbi_arg *args);

void syscall_task_return();


#endif /* syscall.h */