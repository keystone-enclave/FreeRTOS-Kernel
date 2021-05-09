#include <stdint.h>
#include <timex.h>

#include "enclave_rl.h"
#include "printf.h"
#include "eapp_utils.h"
#include "msg_test.h"

char send_buf[DATA_SIZE] = "alex";
char recv_buf[DATA_SIZE];

void EAPP_ENTRY eapp_entry(int SENDER_TID){

    // int x = 5; 
    // int y = 3; 

    sbi_recv(SENDER_TID, recv_buf, DATA_SIZE, YIELD);
    // while(sbi_recv(SENDER_TID, recv_buf, DATA_SIZE, YIELD));
    sbi_send(SENDER_TID, send_buf, DATA_SIZE, YIELD); 
    syscall_task_return();
}