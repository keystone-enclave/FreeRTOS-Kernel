
#include <stdint.h>
#include <timex.h>

#include "simulator.h"
#include "printf.h"
#include "eapp_utils.h"

#define DRIVER_TID 2

#define RAND_MAX 2147483647
static unsigned long my_rand_state = 1;


long rand()
{
    my_rand_state = (my_rand_state * 1103515245 + 12345) % 2147483648;
    return my_rand_state;
}



double q_table[Q_STATE][N_ACTION] = {0};
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

void print_q_table()
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

void send_finish(){
    struct send_action_args args;
    args.msg_type = FINISH;
    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args));
}

void send_env_reset(){
    struct send_action_args args;
    args.msg_type = RESET;
    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args));
    while(sbi_recv(DRIVER_TID, &args, sizeof(struct send_action_args)));
}

void send_env_step(struct probability_matrix_item *next, int action){

    struct send_action_args args;
    args.action = action;
    args.msg_type = STEP;

    sbi_send(DRIVER_TID, &args, sizeof(struct send_action_args)); 

    while(sbi_recv(DRIVER_TID, &args, sizeof(struct send_action_args)));

    next->ctx.done = args.next.ctx.done;
    next->ctx.new_state = args.next.ctx.new_state;
    next->ctx.reward = args.next.ctx.reward;

    

}

void EAPP_ENTRY eapp_entry()
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

    double e_greedy = E_GREEDY;

    int i, j;
    for (i = 0; i < NUM_EPISODES; i++)
    {

        done = FALSE;
        rewards_current_episode = 0;
        state = 0;

        send_env_reset();

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

            send_env_step(&next, action);


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

    send_finish();
    cycles_t et = get_cycles();
    printf("Agent End Time: %u\nAgent Duration: %u\n", et, et - st);
    syscall_task_return();
}
