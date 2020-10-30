#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 

extern uintptr_t __stack_top;

int main( void )
{

        printf("Free RTOS booted!\n"); 
        vTaskStartScheduler();
	return 1; 
}

int interrupt_handler(){
		return 0; 
}
