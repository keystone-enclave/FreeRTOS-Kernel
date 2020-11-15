#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 
#include <csr.h>
#include <sbi.h>
#include <syscall.h> 
#include <enclave.h> 
#include <elf.h> 
#include <stdio.h>

#define HEAP_SIZE  0x800 

extern uintptr_t __stack_top;
extern uintptr_t __heap_base; 
extern void freertos_risc_v_trap_handler(void); 

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

static void taskTestFn(void *pvParameters); 
static void taskTestFn2(void *pvParameters);
TaskHandle_t taskTest; 
TaskHandle_t taskTest2;

TaskHandle_t enclave1; 
TaskHandle_t enclave2; 


int main( void )
{

   extern uintptr_t _rtos_base; 
   extern uintptr_t _rtos_end; 

   extern char * _task_1_start; 
   extern char *_task_1_end; 

   extern char * _task_2_start; 
   extern char *_task_2_end; 


   size_t elf_size = (char *) &_task_1_end - (char *) &_task_1_start;


   printf("Free RTOS booted at 0x%p-0x%p!\n", &_rtos_base, &_rtos_end); 
   printf("Enclave 1 at 0x%p-0x%p!\n", &_task_1_start, &_task_1_end); 
   printf("Enclave 2 at 0x%p-0x%p!\n", &_task_2_start, &_task_2_end);

   xTaskCreateEnclave((uintptr_t) &_task_1_start, elf_size, "fibonacci", 30, &enclave1); 
   xTaskCreateEnclave((uintptr_t) &_task_2_start, elf_size, "attest", 30, &enclave2); 
   
   xTaskCreate(taskTestFn, "taskTest1", configMINIMAL_STACK_SIZE, NULL, 25, &taskTest); 
   xTaskCreate(taskTestFn2, "taskTest2", configMINIMAL_STACK_SIZE, NULL, 21, &taskTest2);
   vTaskStartScheduler();

   //Should not reach here! 
	while(1){
		printf("STUCK!\n"); 
	}
	return 1; 
}

static void taskTestFn(void *pvParameters){
   printf("Untrusted Task 1 DONE\n"); 
   syscall_task_return();
}

static void taskTestFn2(void *pvParameters){
   printf("Untrusted Task 2 before yield\n");
   syscall_task_yield(); 
   printf("Untrusted Task 2 DONE\n");
   syscall_task_return();
}