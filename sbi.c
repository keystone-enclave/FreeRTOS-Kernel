#include "sbi.h"
#include "csr.h"

void
sbi_putchar(char character) {
  SBI_CALL_1(SBI_CONSOLE_PUTCHAR, character);
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
sbi_switch_task(){
  //  SB/I_CALL_0(SBI_SWITCH_TASK); 
}

void
sbi_enable_interrupts(){
  //  SBI_CALL_1(SBI_ENABLE_INTERRUPT, 1); 
}

void
sbi_disable_interrupts(){
  //  SBI_CALL_1(SBI_ENABLE_INTERRUPT, 0); 
}

void ENABLE_INTERRUPTS(void){

   csr_set(sstatus, SR_SIE);
}

void DISABLE_INTERRUPTS(void){
  
  csr_clear(sstatus, SR_SIE);
}
