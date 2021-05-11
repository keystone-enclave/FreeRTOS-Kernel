#include <stdint.h>
#include <timex.h>

#include "enclave_rl.h"
#include "string.h"
#include "printf.h"
#include "eapp_utils.h"
#include "msg_test.h"

// extern char *_shared_buffer_start;
#define SHARED_BUF 0x80441000

uintptr_t shared_buf_sender =(uintptr_t) ((uintptr_t)SHARED_BUF + 512);
uintptr_t shared_buf_receiver =(uintptr_t) SHARED_BUF;

char send_buf[DATA_SIZE] = "hello";
char recv_buf[DATA_SIZE];

void EAPP_ENTRY eapp_entry(int SENDER_TID){

    // int x = 5; 
    // int y = 3; 

    char *shared_buf_receiver_cast = (char *) shared_buf_receiver;

    while(!shared_buf_receiver_cast[0]);
    memcpy((void *) shared_buf_sender, send_buf, DATA_SIZE);
    // memcpy(recv_buf,(void *) shared_buf_receiver, DATA_SIZE);
    sbi_recv(SENDER_TID, 0, 0, YIELD);
    // while(sbi_recv(SENDER_TID, recv_buf, DATA_SIZE, YIELD));
    // sbi_send(SENDER_TID, send_buf, DATA_SIZE, YIELD); 
    syscall_task_return();
}