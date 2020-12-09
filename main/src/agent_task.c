
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

static unsigned long my_rand_state = 1;

long rand()
{
    my_rand_state = (my_rand_state * 1103515245 + 12345) % 2147483648;
    return my_rand_state;
}

int finish = 0;

struct context
{
    int reward;
    int done;
    struct pos next_pos;
};

int max_action(double *actions)
{
    double max = 0.0;
    int max_idx = 0;

    for (int i = 0; i < N_ACTION; i++)
    {
        if (actions[i] > max)
        {
            max = actions[i];
            max_idx = i;
        }
    }
    return max_idx;
}

double max(double *actions)
{
    double ret = 0.0;
    for (int i = 0; i < N_ACTION; i++)
    {
        if (actions[i] > ret)
        {
            ret = actions[i];
        }
    }
    return ret;
}

void print_q_table(double **q_table)
{

    for (int i = 0; i < Q_STATE_N; i++)
    {
        for (int j = 0; j < Q_STATE_N; j++)
        {

            printf("| ");
            for (int k = 0; k < N_ACTION; k++)
            {
                printf("%.2f ", q_table[i * Q_STATE_N + j][k]);
            }
            printf("| ");
        }
        printf("\n");
    }
    printf("\n");
}

void send_finish(QueueHandle_t send_queue){
    struct send_action_args send_action;
    send_action.msg_type = FINISH;

    struct send_action_args *args = &send_action;
    xQueueSend(send_queue, &args, QUEUE_MAX_DELAY);    
}

void send_env_reset(QueueHandle_t send_queue, QueueHandle_t recv_queue){
    struct send_action_args send_action;
    send_action.msg_type = RESET;

    struct send_action_args *args = &send_action;
    xQueueSend(send_queue, &args, QUEUE_MAX_DELAY );
    xQueueReceive(recv_queue, &args, QUEUE_MAX_DELAY);
}

void send_env_step(QueueHandle_t send_queue, QueueHandle_t recv_queue, struct probability_matrix_item *next, int action){

    struct send_action_args send_action;
    send_action.action = action;
    send_action.msg_type = STEP;

    struct send_action_args *args = &send_action;

    xQueueSend(send_queue, &args, QUEUE_MAX_DELAY);
    xQueueReceive(recv_queue, &args, QUEUE_MAX_DELAY);

    // sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args)); 
    // while(sbi_recv(DRIVER_TID, &args, sizeof(struct send_action_args)));

    next->ctx.done = args->next.ctx.done;
    next->ctx.new_state = args->next.ctx.new_state;
    next->ctx.reward = args->next.ctx.reward;
}

