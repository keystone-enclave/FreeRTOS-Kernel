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
#include <rl_config.h>
#include <timex.h>

#include "FreeRTOS_IO.h"
#include "CLI_functions.h"
#include "console.h"
#include "commands.h"
#include "devices.h"

#define HEAP_SIZE 0x800

extern uintptr_t __stack_top;
extern uintptr_t __heap_base;
extern void freertos_risc_v_trap_handler(void);

extern void aes_task(void *pvParameters);
extern void dhrystone_task(void *pvParameters);
extern void miniz_task(void *pvParameters);
extern void norx_task(void *pvParameters);
extern void primes_task(void *pvParameters);
extern void qsort_task(void *pvParameters);
extern void sha512_task(void *pvParameters);

uint8_t ucHeap[configTOTAL_HEAP_SIZE * 4] = {0};

#ifdef TEST
TaskHandle_t taskTest1 = 0;
TaskHandle_t taskTest2 = 0;

static void taskTestFn1(void *pvParameters);
static void taskTestFn2(void *pvParameters);
#endif

QueueHandle_t xQueue = 0;
QueueHandle_t senderQueue = 0;
QueueHandle_t receiverQueue = 0;

#ifdef MSG_TEST_ENCLAVE
static void sender_task(void *pvParameters);
#endif

#ifdef MSG_TEST_TASK
static void sender_fn(void *pvParameters);
static void receiver_fn(void *pvParameters);
#endif


#ifdef TA_TD_RL
TaskHandle_t agent = 0;
TaskHandle_t driver = 0;

QueueHandle_t xAgentQueue = 0;
QueueHandle_t xDriverQueue = 0;

static void agent_task(void *pvParameters);
static void driver_task(void *pvParameters);
#endif

TaskHandle_t taskCLI = 0;

#ifdef RV8
TaskHandle_t enclave_rv8 = 0;
#endif

#ifdef ENCLAVE
TaskHandle_t enclave1 = 0;
TaskHandle_t enclave2 = 0;
#endif

#ifdef EA_ED_RL
TaskHandle_t enclave3 = 0;
TaskHandle_t enclave4 = 0;
#endif

#ifdef TA_ED_RL
TaskHandle_t agent;
TaskHandle_t enclave4;
static void agent_task(void *pvParameters);
#endif

#ifdef EA_TD_RL
TaskHandle_t driver = 0;
TaskHandle_t enclave3 = 0;
static void driver_task(void *pvParameters);
#endif

Peripheral_Descriptor_t uart = 0;

extern void xPortTaskReturn(int ret_code);

int main(void)
{
    cycles_t st = get_cycles();
    cycles_t et = 0;
    printf("Setup Start Time: %u\n", st);
    /* Register devices with the FreeRTOS+IO. */
    vRegisterIODevices();

    xQueue = xQueueCreate(10, sizeof(uintptr_t));

    uart = FreeRTOS_open("/dev/uart", 0);
    
#ifdef RTOS_DEBUG
    extern uintptr_t _rtos_base;
    extern uintptr_t _rtos_end;
    printf("Free RTOS booted at 0x%p-0x%p!\n", &_rtos_base, &_rtos_end);
#endif

#ifdef TEST
    xTaskCreate(taskTestFn1, "task1", configMINIMAL_STACK_SIZE, (void *)5, 25, &taskTest1);
    xTaskCreate(taskTestFn2, "task2", configMINIMAL_STACK_SIZE, NULL, 28, &taskTest2);
#endif

#ifdef RV8
    extern char *_rv8_start;
    extern char *_rv8_end;
    size_t elf_size_rv8 = (char *)&_rv8_end - (char *)&_rv8_start;

    printf("RV8 Test at 0x%p-0x%p!\n", &_rv8_start, &_rv8_end);
    xTaskCreateEnclave((uintptr_t)&_rv8_start, elf_size_rv8, "rv8", 30, NULL, &enclave_rv8);



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

#ifdef EA_ED_RL
    printf("Running Agent Enclave and Driver Enclave Test...\n");

    extern char *_agent_start;
    extern char *_agent_end;

    extern char *_simulator_start;
    extern char *_simulator_end;

    size_t elf_size_3 = (char *)&_agent_end - (char *)&_agent_start;
    size_t elf_size_4 = (char *)&_simulator_end - (char *)&_simulator_start;

#ifdef RTOS_DEBUG
    printf("Agent at 0x%p-0x%p!\n", &_agent_start, &_agent_end);
    printf("Simulator at 0x%p-0x%p!\n", &_simulator_start, &_simulator_end);
#endif

    xTaskCreateEnclave((uintptr_t)&_agent_start, elf_size_3, "agent", 30, (void *const)DRIVER_TID, &enclave3);
    xTaskCreateEnclave((uintptr_t)&_simulator_start, elf_size_4, "simulator", 30, (void *const)AGENT_TID, &enclave4);
#endif

#ifdef TA_ED_RL
    extern char *_simulator_start;
    extern char *_simulator_end;

    printf("Running Agent Task and Driver Enclave Test...\n");
    size_t elf_size_4 = (char *)&_simulator_end - (char *)&_simulator_start;

#ifdef RTOS_DEBUG
    printf("Simulator at 0x%p-0x%p!\n", &_simulator_start, &_simulator_end);
#endif

    xTaskCreate(agent_task, "agent", configMINIMAL_STACK_SIZE * 6, NULL, 30, &agent);
    xTaskCreateEnclave((uintptr_t)&_simulator_start, elf_size_4, "simulator", 30, (void *const) AGENT_TID ,&enclave4);
#endif

#ifdef EA_TD_RL
    extern char *_agent_start;
    extern char *_agent_end;
    
    printf("Running Agent Enclave and Driver Task Test...\n");

    size_t elf_size_3 = (char *)&_agent_end - (char *)&_agent_start;

#ifdef RTOS_DEBUG
    printf("Agent at 0x%p-0x%p!\n", &_agent_start, &_agent_end);
#endif
    xTaskCreateEnclave((uintptr_t)&_agent_start, elf_size_3, "agent", 30, (void * const) DRIVER_TID, &enclave3);
    xTaskCreate(driver_task, "driver", configMINIMAL_STACK_SIZE * 4, NULL, 30, &driver);
#endif

#ifdef TA_TD_RL
    printf("Running Agent Task and Driver Task Test...\n");

    xAgentQueue = xQueueCreate(10, sizeof(uintptr_t));
    xDriverQueue = xQueueCreate(10, sizeof(uintptr_t));

    xTaskCreate(agent_task, "agent", configMINIMAL_STACK_SIZE * 6, NULL, 25, &agent);
    xTaskCreate(driver_task, "driver", configMINIMAL_STACK_SIZE * 4, NULL, 25, &driver);
#endif

#ifdef MSG_TEST_ENCLAVE
    printf("Running Message Enclave Test...\n");

    TaskHandle_t enclave_receiver = 0;
    TaskHandle_t task_sender;

    extern char *_receiver_start;
    extern char *_receiver_end;

    size_t elf_size_4 = (char *)&_receiver_end - (char *)&_receiver_start;
     printf("Receiver at 0x%p-0x%p!\n", &_receiver_start, &_receiver_end);
    
    xTaskCreate(sender_task, "sender", configMINIMAL_STACK_SIZE * 6, NULL, 30, &task_sender);
    xTaskCreateEnclave((uintptr_t)&_receiver_start, elf_size_4, "receiver", 30, (void *const)0, &enclave_receiver);

#endif

#ifdef MSG_TEST_TASK
    TaskHandle_t sender = 0;
    TaskHandle_t receiver = 0;

    senderQueue = xQueueCreate(10, sizeof(uintptr_t));
    receiverQueue = xQueueCreate(10, sizeof(uintptr_t));

    xTaskCreate(sender_fn, "sender", configMINIMAL_STACK_SIZE * 6, NULL, 25, &sender);
    xTaskCreate(receiver_fn, "receiver", configMINIMAL_STACK_SIZE * 4, NULL, 25, &receiver);
#endif

    // xTaskCreate(aes_task, "aes", configMINIMAL_STACK_SIZE, NULL, 30, &taskCLI);
    // xTaskCreate(dhrystone_task, "dhrystone", configMINIMAL_STACK_SIZE, NULL, 29, &taskCLI);
    // xTaskCreate(miniz_task, "miniz", configMINIMAL_STACK_SIZE, NULL, 28, &taskCLI);
    // xTaskCreate(norx_task, "norx", configMINIMAL_STACK_SIZE, NULL, 27, &taskCLI);
    // xTaskCreate(primes_task, "primes", configMINIMAL_STACK_SIZE, NULL, 26, &taskCLI);
    // xTaskCreate(qsort_task, "qsort", configMINIMAL_STACK_SIZE, NULL, 25, &taskCLI);
    // xTaskCreate(sha512_task, "sha512", configMINIMAL_STACK_SIZE, NULL, 24, &taskCLI);

    BaseType_t bt = xTaskCreate(vCommandConsoleTask, "CLI", configMINIMAL_STACK_SIZE, (void *)uart, 2, &taskCLI);

    printf("cli %i", bt);

    /* Register commands with the FreeRTOS+CLI command interpreter. */
    vRegisterCLICommands();

    /* Start the tasks and timer running. */
    et = get_cycles();
    printf("Setup End Time: %u\nDuration: %u\n", et, et - st);
    vTaskStartScheduler();

    //Should not reach here!
    while (1)
    {
        printf("STUCK!\n");
    }
    return 1;
}

#ifdef MSG_TEST_ENCLAVE
static void sender_task(void *pvParameters)
{
    #define DATA_SIZE 512
    char send_buf[DATA_SIZE] = "hello";
    char recv_buf[DATA_SIZE];

    // int x = 3; 
    // int y = 5; 

    cycles_t st = get_cycles();
    cycles_t et;

    sbi_send(1, send_buf, DATA_SIZE, YIELD); 
    sbi_recv(1, recv_buf, DATA_SIZE, YIELD);
    // while(sbi_recv(1, recv_buf, DATA_SIZE, YIELD)) yield_general(); 
    et = get_cycles();
    printf("h ==? %c \n", recv_buf[0]); 
    printf("[msg-test-enclave] Duration: %lu\n", et -st);
    return_general();
}
#endif


#ifdef MSG_TEST_TASK
static void sender_fn(void *pvParameters)
{
    cycles_t msg_start = get_cycles();
    cycles_t msg_end = 0;
    int send_msg = 111;
    int recv_msg = 123;

    xQueueSend(receiverQueue, &send_msg, 100000);
    xQueueReceive(senderQueue, &recv_msg, 100000);

    msg_end = get_cycles();

    printf("[msg-test-task] Duration: %lu\n", msg_end - msg_start); 
    return_general(); 

}

static void receiver_fn(void *pvParameters)
{
    int send_msg; 
    int recv_msg = 123;

    xQueueReceive(receiverQueue, &send_msg, 100000);
    xQueueSend(senderQueue, &recv_msg, 100000);
    return_general(); 
}
#endif

#ifdef TEST

static void taskTestFn1(void *pvParameters)
{
    printf("Your number is: %p\n", pvParameters);

    int x;

    // xTaskNotifyGive(taskTest2);

    ulTaskNotifyTake(pdTRUE, 100000);
    xQueueReceive(xQueue, &x, 100000);

    printf("[Receiver]: %d\n", x);
    printf("Untrusted Task 1 DONE\n");
    xPortTaskReturn(RET_EXIT);
}

static void taskTestFn2(void *pvParameters)
{
    printf("Untrusted Task 2 before yield\n");
    int send = 1337;
    // xPortReturn(RET_YIELD);
    // ulTaskNotifyTake( pdTRUE, 100000 );
    xQueueSend(xQueue, &send, 100000);
    // xTaskNotifyGive( taskTest );
    printf("Untrusted Task 2 DONE\n");
    printf("Task Runtime: %u\n", xTaskGetTickCount());
    xPortTaskReturn(RET_EXIT);
}

#endif


#ifdef TA_ED_RL
void eapp_send_finish()
{
    struct send_action_args args;
    args.msg_type = FINISH;
    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args), YIELD);
}

void eapp_send_env_reset()
{

    struct send_action_args args;
    int reset_ack;
    args.msg_type = RESET;
    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args), YIELD);

    int recv_msg = sbi_recv(DRIVER_TID, &reset_ack, sizeof(int), YIELD);

    while (recv_msg)
    {
        yield_general(); 
        recv_msg = sbi_recv(DRIVER_TID, &reset_ack, sizeof(int), YIELD);
    }
}

void eapp_send_env_step(struct probability_matrix_item *next, int action)
{
    struct send_action_args args;
    struct ctx ctx_buf; 
    args.action = action;
    args.msg_type = STEP;

    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args), YIELD);

    while (sbi_recv(DRIVER_TID, &ctx_buf, sizeof(struct ctx), YIELD))
    {
        yield_general();
    }

    next->ctx.done = ctx_buf.done;
    next->ctx.new_state = ctx_buf.new_state;
    next->ctx.reward = ctx_buf.reward;
}
static void agent_task(void *pvParameters)
{
    cycles_t st = get_cycles();
    printf("Agent Start Time: %u\n", st);
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

        eapp_send_env_reset();

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

            eapp_send_env_step(&next, action);

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
 #ifdef DEBUG
            printf("Episode: %d, Steps taken: %d, Reward: %d\n", i, j, rewards_current_episode);
 #endif
        }
    }

    eapp_send_finish();
    cycles_t et = get_cycles();
    printf("Agent End Time: %u\nAgent Duration: %u\n", et, et - st);
    return_general(); 
}
#endif

#ifdef EA_TD_RL
static void driver_task(void *pvParameters)
{
    printf("Enter Simulator\n");
    env_setup();
    struct send_action_args args;
    struct probability_matrix_item p_item;
    int reset_ack = 1; 

    while (1)
    {
        while (sbi_recv(AGENT_TID, &args, sizeof(struct send_action_args), YIELD));
        switch (args.msg_type)
        {
        case RESET:
            env_reset();
            sbi_send(AGENT_TID, &reset_ack, sizeof(int),YIELD);
            break;
        case STEP:
            step(&p_item, args.action);
            sbi_send(AGENT_TID, &p_item.ctx, sizeof(struct ctx),YIELD);
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
    return_general();
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