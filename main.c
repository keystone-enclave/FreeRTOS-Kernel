#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 
#include <csr.h>
#include <sbi.h>
#include <syscall.h> 

#define HEAP_SIZE  0x800 

extern uintptr_t __stack_top;
extern uintptr_t __heap_base; 
extern void freertos_risc_v_trap_handler(void); 

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

static void taskTestFn(void *pvParameters); 
static void taskTestFn2(void *pvParameters);
TaskHandle_t taskTest; 
TaskHandle_t taskTest2;


int main( void )
{
        printf("Free RTOS booted!\n"); 

        xTaskCreate(taskTestFn, "taskTest1", configMINIMAL_STACK_SIZE, NULL, 20, &taskTest); 
        xTaskCreate(taskTestFn2, "taskTest2", configMINIMAL_STACK_SIZE, NULL, 21, &taskTest2);
        vTaskStartScheduler();


	while(1){
		printf("STUCK!\n"); 
	}
	return 1; 
}

static void taskTestFn(void *pvParameters){
   printf("Task 1\n"); 
   syscall_task_return();
}

static void taskTestFn2(void *pvParameters){
   printf("Task 2\n");
   syscall_task_return();
}