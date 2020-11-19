
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

int
syscall_getchar() {
  return SBI_CALL_0(SBI_CONSOLE_GETCHAR);
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

int interrupt_handler(struct task_ctx *regs){

    handle_timer_interrupt();
	return 1; 
}