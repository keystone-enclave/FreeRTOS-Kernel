#include <FreeRTOS.h>
#include <task.h>
#include <printf.h> 
#include <csr.h>
#include <sbi.h>
#include <syscall.h> 
#include <enclave.h> 
#include <elf.h> 
#include <stdio.h>
#include <string.h>
#include <queue.h>
#include <rl.h>

#include "FreeRTOS_IO.h"
#include "console.h"
#include "commands.h"
#include "devices.h"

#define HEAP_SIZE  0x800 
// #define RL_TEST
#define ENCLAVE_RL
// #define TEST


extern uintptr_t __stack_top;
extern uintptr_t __heap_base; 
extern void freertos_risc_v_trap_handler(void); 

uint8_t ucHeap[ configTOTAL_HEAP_SIZE * 4 ] = {0};

#ifdef TEST
    TaskHandle_t taskTest1 = 0; 
    TaskHandle_t taskTest2 = 0;

    static void taskTestFn1(void *pvParameters); 
    static void taskTestFn2(void *pvParameters);
#endif

QueueHandle_t xQueue = 0;

#ifdef RL_TEST
    TaskHandle_t agent = 0; 
    TaskHandle_t driver = 0;

    QueueHandle_t xAgentQueue = 0;
    QueueHandle_t xDriverQueue = 0;

    static void agent_task(void *pvParameters);
    static void driver_task(void *pvParameters);
#endif

TaskHandle_t taskCLI = 0;

#ifdef ENCLAVE
    TaskHandle_t enclave1 = 0; 
    TaskHandle_t enclave2 = 0; 
#endif

#ifdef ENCLAVE_RL
    TaskHandle_t enclave3 = 0; 
    TaskHandle_t enclave4 = 0; 
#endif

Peripheral_Descriptor_t uart = 0;


extern void xPortTaskReturn(int ret_code);



int main( void )
{
   /* Register devices with the FreeRTOS+IO. */
	vRegisterIODevices();

   xQueue = xQueueCreate(10, sizeof(uintptr_t));

   uart = FreeRTOS_open("/dev/uart", 0);

   printf("UART: %p\n", uart);

   extern uintptr_t _rtos_base; 
   extern uintptr_t _rtos_end; 

    printf("Free RTOS booted at 0x%p-0x%p!\n", &_rtos_base, &_rtos_end); 

#ifdef TEST
   xTaskCreate(taskTestFn1, "task1", configMINIMAL_STACK_SIZE, (void *)5, 25, &taskTest1);
   xTaskCreate(taskTestFn2, "task2", configMINIMAL_STACK_SIZE, NULL, 28, &taskTest2);
#endif

#ifdef ENCLAVE
   extern char *_task_1_start;
   extern char *_task_1_end;

   extern char *_task_2_start;
   extern char *_task_2_end;

   size_t elf_size_1 = (char *)&_task_1_end - (char *)&_task_1_start;
   size_t elf_size_2 = (char *)&_task_2_end - (char *)&_task_2_start;

   printf("Enclave 1 at 0x%p-0x%p!\n", &_task_1_start, &_task_1_end);
   printf("Enclave 2 at 0x%p-0x%p!\n", &_task_2_start, &_task_2_end);

   xTaskCreateEnclave((uintptr_t)&_task_1_start, elf_size_1, "fibonacci", 30, &enclave1);
   xTaskCreateEnclave((uintptr_t)&_task_2_start, elf_size_2, "attest", 30, &enclave2);

#endif

#ifdef ENCLAVE_RL
   extern char *_agent_start;
   extern char *_agent_end;

   extern char *_simulator_start;
   extern char *_simulator_end;

   size_t elf_size_3 = (char *)&_agent_end - (char *)&_agent_start;
   size_t elf_size_4 = (char *)&_simulator_end - (char *)&_simulator_start;

   printf("Agent at 0x%p-0x%p!\n", &_agent_start, &_agent_end);
   printf("Simulator at 0x%p-0x%p!\n", &_simulator_start, &_simulator_end);

   xTaskCreateEnclave((uintptr_t)&_agent_start, elf_size_3, "agent", 30, &enclave3);
   xTaskCreateEnclave((uintptr_t)&_simulator_start, elf_size_4, "simulator", 30, &enclave4);
#endif

#ifdef RL_TEST
   xAgentQueue = xQueueCreate(10, sizeof(uintptr_t));
   xDriverQueue = xQueueCreate(10, sizeof(uintptr_t));

   xTaskCreate(agent_task, "agent", configMINIMAL_STACK_SIZE * 6, NULL, 25, &agent);
   xTaskCreate(driver_task, "driver", configMINIMAL_STACK_SIZE * 4, NULL, 21, &driver);
#endif

   xTaskCreate(vCommandConsoleTask, "CLI", configMINIMAL_STACK_SIZE, (void *)uart, 1, &taskCLI);

   /* Register commands with the FreeRTOS+CLI command interpreter. */
   vRegisterCLICommands();

   /* Start the tasks and timer running. */
   vTaskStartScheduler();

   //Should not reach here!
//    while (1)
//    {
//        printf("STUCK!\n");
//    }
   return 1;
}

#ifdef TEST

static void taskTestFn1(void *pvParameters){
   printf("Your number is: %i\n", (int)pvParameters);

   int x; 

   // xTaskNotifyGive(taskTest2);

   // ulTaskNotifyTake( pdTRUE, 100000 );
   xQueueReceive( xQueue, &x, 100000 );

   printf("[Receiver]: %d\n", x);
   printf("Untrusted Task 1 DONE\n"); 
   xPortTaskReturn(RET_EXIT);
}

static void taskTestFn2(void *pvParameters){
   printf("Untrusted Task 2 before yield\n");
   int send = 1337; 
   // xPortReturn(RET_YIELD);
   // ulTaskNotifyTake( pdTRUE, 100000 );
   xQueueSend( xQueue, &send, 100000 );
   // xTaskNotifyGive( taskTest );
   printf("Untrusted Task 2 DONE\n");
   xPortTaskReturn(RET_EXIT);
}

#endif


#ifdef RL_TEST
static void agent_task(void *pvParameters){
    printf("Enter Agent\n");
    int action;
    int state;
    int new_state;
    int reward;
    int done;
    int rewards_current_episode = 0;
    struct probability_matrix_item next;
    double q_table[Q_STATE][N_ACTION] = {0};
    double e_greedy = E_GREEDY;

    int i, j;
    for (i = 0; i < NUM_EPISODES; i++)
    {

        done = FALSE;
        rewards_current_episode = 0;
        state = 0;

        send_env_reset(xDriverQueue, xAgentQueue);

        for (j = 0; j < STEPS_PER_EP; j++)
        {

            float random_f = (float)rand() / (float)(RAND_MAX / 1.0);

            if (random_f > e_greedy)
            {
                action = max_action(q_table[state]);
            }
            else
            {
                action = rand() % 4;
            }

            send_env_step(xDriverQueue, xAgentQueue, &next, action);


            new_state = next.ctx.new_state;
            reward = next.ctx.reward;
            done = next.ctx.done;

            q_table[state][action] = q_table[state][action] * (1.0 - LEARN_RATE) + LEARN_RATE * (reward + DISCOUNT * max(q_table[new_state]));

            state = new_state;
            rewards_current_episode += reward;

            if (done == TRUE)
            {

                if (reward == 1)
                {
                    if (e_greedy > E_GREEDY_F)
                    e_greedy *= E_GREEDY_DECAY;
#ifdef DEBUG
                    printf("You reached the goal!\n");
#endif
                }
                else
                {
#ifdef DEBUG
                    printf("You fell in a hole!\n");
#endif
                }
                break;
            }
        }
        if (i % 10 == 0)
        {
            printf("Episode: %d, Steps taken: %d, Reward: %d\n", i, j, rewards_current_episode);
        }
    }

    send_finish(xDriverQueue);
    xPortTaskReturn(RET_EXIT);
}


static void driver_task(void *pvParameters){
    printf("Enter Simulator\n");
    env_setup();
    struct send_action_args *args;

    while(1){

      xQueueReceive(xDriverQueue, &args, QUEUE_MAX_DELAY);
        
      switch(args->msg_type){
         case RESET:
               env_reset();
               xQueueSend(xAgentQueue, &args, QUEUE_MAX_DELAY);
               break;
         case STEP:
               step(&args->next, args->action);
               xQueueSend(xAgentQueue, &args, QUEUE_MAX_DELAY);               
               break;
         case FINISH:
               goto done;
               break;
         default:
               printf("Invalid message type!\n");
               break;
      }
    }

done:
    xPortTaskReturn(RET_EXIT);
}
#endif

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