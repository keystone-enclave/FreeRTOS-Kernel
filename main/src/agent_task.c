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
    struct ctx ctx_recv; 
    send_action.action = action;
    send_action.msg_type = STEP;

    struct send_action_args *args = &send_action;
    struct ctx *ctx_buf = &ctx_recv;

    xQueueSend(send_queue, &args, QUEUE_MAX_DELAY);
    xQueueReceive(recv_queue, &ctx_buf, QUEUE_MAX_DELAY);

    next->ctx.done = ctx_buf->done;
    next->ctx.new_state = ctx_buf->new_state;
    next->ctx.reward = ctx_buf->reward;
}

#ifdef TA_TD_RL
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
    cycles_t et = get_cycles();
    printf("Agent End Time: %u\nAgent Duration: %u\n", et, et - st);
    return_general();
}

#endif