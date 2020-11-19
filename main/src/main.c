#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 
#include <csr.h>
#include <sbi.h>
#include <syscall.h> 
#include <enclave.h> 
#include <elf.h> 
#include <stdio.h>

#include "console.h"
#include "commands.h"

#define HEAP_SIZE  0x800 

extern uintptr_t __stack_top;
extern uintptr_t __heap_base; 
extern void freertos_risc_v_trap_handler(void); 

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

static void taskTestFn(void *pvParameters); 
static void taskTestFn2(void *pvParameters);
TaskHandle_t taskTest; 
TaskHandle_t taskTest2;
TaskHandle_t taskCLI;

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

   xTaskCreate( vCommandConsoleTask, "CLI", configMINIMAL_STACK_SIZE, NULL, 1, &taskCLI );

   /* Register commands with the FreeRTOS+CLI command interpreter. */
	vRegisterCLICommands();

	/* Start the tasks and timer running. */
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

/*-----------------------------------------------------------*/

// void vMainConfigureTimerForRunTimeStats( void )
// {
// 	/* How many clocks are there per tenth of a millisecond? */
// 	ulClocksPer10thOfAMilliSecond = configCPU_CLOCK_HZ / 10000UL;
// }
// /*-----------------------------------------------------------*/

// uint32_t ulMainGetRunTimeCounterValue( void )
// {
// uint32_t ulSysTickCounts, ulTickCount, ulReturn;
// const uint32_t ulSysTickReloadValue = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
// volatile uint32_t * const pulCurrentSysTickCount = ( ( volatile uint32_t *) 0xe000e018 );
// volatile uint32_t * const pulInterruptCTRLState = ( ( volatile uint32_t *) 0xe000ed04 );
// const uint32_t ulSysTickPendingBit = 0x04000000UL;

// 	/* NOTE: There are potentially race conditions here.  However, it is used
// 	anyway to keep the examples simple, and to avoid reliance on a separate
// 	timer peripheral. */


// 	/* The SysTick is a down counter.  How many clocks have passed since it was
// 	last reloaded? */
// 	ulSysTickCounts = ulSysTickReloadValue - *pulCurrentSysTickCount;

// 	/* How many times has it overflowed? */
// 	ulTickCount = xTaskGetTickCountFromISR();

// 	/* Is there a SysTick interrupt pending? */
// 	if( ( *pulInterruptCTRLState & ulSysTickPendingBit ) != 0UL )
// 	{
// 		/* There is a SysTick interrupt pending, so the SysTick has overflowed
// 		but the tick count not yet incremented. */
// 		ulTickCount++;

// 		/* Read the SysTick again, as the overflow might have occurred since
// 		it was read last. */
// 		ulSysTickCounts = ulSysTickReloadValue - *pulCurrentSysTickCount;
// 	}

// 	/* Convert the tick count into tenths of a millisecond.  THIS ASSUMES
// 	configTICK_RATE_HZ is 1000! */
// 	ulReturn = ( ulTickCount * 10UL ) ;

// 	/* Add on the number of tenths of a millisecond that have passed since the
// 	tick count last got updated. */
// 	ulReturn += ( ulSysTickCounts / ulClocksPer10thOfAMilliSecond );

// 	return ulReturn;
// }
/*-----------------------------------------------------------*/

// void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
// {
// 	( void ) pcTaskName;
// 	( void ) pxTask;

// 	/* Run time stack overflow checking is performed if
// 	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
// 	function is called if a stack overflow is detected. */
// 	taskDISABLE_INTERRUPTS();
// 	for( ;; );
// }
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/