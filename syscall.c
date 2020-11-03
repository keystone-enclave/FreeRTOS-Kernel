
#include "syscall.h"
#include "regs.h"
#include "printf.h"

void
syscall_putchar(char character) {
  SBI_CALL_1(SYSCALL_CONSOLE_PUTCHAR, character);
}

void syscall_yield(){
    SBI_CALL_0(SYSCALL_YIELD); 
}

void syscall_task_delete(){
    SBI_CALL_0(SYSCALL_TASK_DELETE); 
}


void handle_syscalls(struct encl_ctx *ctx) {

    extern void vTaskSwitchContext( void );
    extern void vTaskDeleteSelf();

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
        case SYSCALL_YIELD:
            vTaskSwitchContext();
            break; 
        case SYSCALL_TASK_DELETE:
            vTaskDeleteSelf(); 
		default:
			break; 
	}

}

void vPortHandleException(struct encl_ctx *regs){

	switch(regs->scause){
       case 8:
            handle_syscalls(regs);
            break;
        default:
            break; 
   }

}

int interrupt_handler(struct encl_ctx *regs){
   
//    switch(regs->scause){
//        case 5:
            handle_timer_interrupt();
//             break;
//         default:
//             break; 
//    }

	return 1; 
}