#include <stdint.h>
#include <timex.h>

#include "enclave_rl.h"
#include "printf.h"
#include "eapp_utils.h"
#include "msg_test.h"

char send_buf[DATA_SIZE];
char recv_buf[DATA_SIZE];

void EAPP_ENTRY eapp_entry(int RECEIVER_TID){

    cycles_t st = get_cycles();
    cycles_t et;

    sbi_send(RECEIVER_TID, send_buf, DATA_SIZE, YIELD); 
    while(sbi_recv(RECEIVER_TID, recv_buf, DATA_SIZE, YIELD));
    et = get_cycles();

    printf("[msg-test-enclave] Duration: %lu\n", et -st);

    syscall_task_return();
}