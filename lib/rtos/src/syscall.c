
#include "FreeRTOS.h"
#include "task.h"
#include "csr.h" 
#include "syscall.h"
#include "timex.h"
#include "regs.h"
#include "printf.h"
#include "portmacro.h"


extern void vTaskSwitchContext( void );
extern void vTaskDeleteSelf();

void
syscall_putchar(char character) {
  SBI_CALL_1(SYSCALL_CONSOLE_PUTCHAR, character);
}

void
sbi_getchar() {
  SBI_CALL_0(SBI_CONSOLE_GETCHAR);
}


uintptr_t syscall_switch_task(uintptr_t task_id, uintptr_t ret_type){

    //Switches to new task 
     return SBI_CALL_2(SBI_SWITCH_TASK, task_id, ret_type); 
}


uintptr_t syscall_register_task (struct register_sbi_arg *args) {
    size_t task_id;
    task_id = SBI_CALL_1(SBI_REGISTER_TASK, args); 
    return task_id; 

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


void handle_syscalls(struct task_ctx *ctx) {


	uintptr_t n = ctx->regs.a7;
	uintptr_t arg0 = ctx->regs.a0;
	// uintptr_t arg1 = ctx->regs.a1;
	// uintptr_t arg2 = ctx->regs.a2;
	// uintptr_t arg3 = ctx->regs.a3;
	// uintptr_t arg4 = ctx->regs.a4;

	// printf("in handler_user_trap %d\n", n); 
	switch(n){
		case SYSCALL_CONSOLE_PUTCHAR:
			sbi_putchar(arg0); 
			break;
        // case SYSCALL_YIELD:
        //     vTaskSwitchContext();
        //     break; 
        // case SYSCALL_TASK_DELETE:
        //     vTaskDeleteSelf(); 
		default:
			break; 
	}

}

void vPortHandleException(struct task_ctx *regs){

// 	switch(regs->scause){
//        case 8:
//             handle_syscalls(regs);
//             break;
//         default:
//             break; 
//    }

}

int interrupt_handler(struct task_ctx *regs){
   
//    switch(regs->scause){
//        case 5:
            handle_timer_interrupt();
//             break;
//         default:
//             break; 
//    }

	return 1; 
}