#include "eapp_utils.h"

void
sbi_putchar(char character) {
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

int sbi_send(int task_id, void *msg, int size){
    return SBI_CALL_3(SBI_SEND_TASK, task_id, msg, size);
}


int sbi_recv(int task_id, void *msg, int size){
    return SBI_CALL_3(SBI_RECV_TASK, task_id, msg, size);
}