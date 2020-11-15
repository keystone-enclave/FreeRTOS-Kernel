#include "eapp_utils.h"
#include "printf.h"

#define MDSIZE  64
#define SIGNATURE_SIZE  64
#define PRIVATE_KEY_SIZE  64 // includes public key
#define PUBLIC_KEY_SIZE 32

typedef unsigned char byte;
#define ATTEST_DATA_MAXLEN  1024

/* attestation reports */
struct enclave_report
{
  byte hash[MDSIZE];
  uint64_t data_len;
  byte data[ATTEST_DATA_MAXLEN];
  byte signature[SIGNATURE_SIZE];
};
struct sm_report
{
  byte hash[MDSIZE];
  byte public_key[PUBLIC_KEY_SIZE];
  byte signature[SIGNATURE_SIZE];
};
struct report
{
  struct enclave_report enclave;
  struct sm_report sm;
  byte dev_public_key[PUBLIC_KEY_SIZE];
};

uintptr_t
sbi_attest_task(void* report, void* buf, uintptr_t len) {
  return SBI_CALL_3(SBI_ATTEST_TASK, report, buf, len);
}


void EAPP_ENTRY eapp_entry(){
    char* data = "nonce";
    char buffer[2048];
    struct report *report; 

    sbi_attest_task((void*) buffer, data, 5);

    report = (struct report *) buffer; 

    printf("[Enclave 2] Enclave hash is...\n");

    for(int i = 0; i < MDSIZE; i++){
        printf("%x", report->enclave.hash[i]); 
    }

    printf("\n");

    printf("[Enclave 2] SM hash is...\n");

    for(int i = 0; i < MDSIZE; i++){
        printf("%x", report->sm.hash[i]); 
    }

    printf("\n");

    printf("[Enclave 2] Signature is...\n");

    for(int i = 0; i < SIGNATURE_SIZE; i++){
        printf("%x", report->enclave.signature[i]); 
    }

    printf("\n");

    printf("[Enclave 2] DONE\n"); 
    syscall_task_return();

}