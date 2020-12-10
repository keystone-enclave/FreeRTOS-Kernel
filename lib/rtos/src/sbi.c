#include "sbi.h"
#include "csr.h"

int
sbi_putchar(int character) {
  return SBI_CALL_1(SBI_CONSOLE_PUTCHAR, character);
}

int
sbi_getchar() {
  return SBI_CALL_0(SBI_CONSOLE_GETCHAR);
}

void
sbi_set_timer(uint64_t stime_value) {
#if __riscv_xlen == 32
  SBI_CALL_2(SBI_SET_TIMER, stime_value, stime_value >> 32);
#else
  SBI_CALL_1(SBI_SET_TIMER, stime_value);
#endif
}

void
sbi_enable_interrupts(){
  //  SBI_CALL_1(SBI_ENABLE_INTERRUPT, 1); 
}

void
sbi_disable_interrupts(){
  //  SBI_CALL_1(SBI_ENABLE_INTERRUPT, 0); 
}

int sbi_send(int task_id, void *msg, int size){
    return SBI_CALL_3(SBI_SEND_TASK, task_id, msg, size);
}


int sbi_recv(int task_id, void *msg, int size){
    return SBI_CALL_3(SBI_RECV_TASK, task_id, msg, size);
}