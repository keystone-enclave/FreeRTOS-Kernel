#include "enclave_rl.h"
#include "printf.h"
#include "eapp_utils.h"

int grid[Q_STATE_N * Q_STATE_N] = {0}; 
static unsigned long dummy_peripheral[1]  = {0}; 
struct probability_matrix_item probability_matrix[Q_STATE_N * Q_STATE_N][N_ACTION] = {0}; 
int curr_state = 0;
int last_action = NO_ACTION;

void init_helper_1(int *grid_row, int filter)
{
    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == filter)
        {
            grid_row[i] = Hole;
        }
        else
        {
            grid_row[i] = Frozen;
        }
    }
}

void initialize_grid_4()
{
    printf("Initializing grid...\n");
    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == 0)
        {
            grid[i] = Start;
        }
        else
        {
            grid[i] = Frozen;
        }
    }

    //Row 1
    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == 1 || i == 3)
        {
            grid[1*Q_STATE_N + i] = Hole;
        }
        else
        {
            grid[1*Q_STATE_N + i] = Frozen;
        }
    }

    //Row 2
    init_helper_1(&grid[2*Q_STATE_N], 3);

    //Row 3
    init_helper_1(&grid[3*Q_STATE_N], 0);

    grid[3*Q_STATE_N + 3] = Goal; 
}

#ifdef EIGHT
void initialize_grid_8()
{

    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == 0)
        {
            grid[0][i] = Start;
        }
        else
        {
            grid[0][i] = Frozen;
        }
    }

    for (int i = 0; i < Q_STATE_N; i++)
    {
        grid[1][i] = Frozen;
    }

    init_helper_1(grid[2], 3);
    init_helper_1(grid[3], 5);
    init_helper_1(grid[4], 3);

    //Row 5
    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == 1 || i == 2 || i == 6)
        {
            grid[5][i] = Hole;
        }
        else
        {
            grid[5][i] = Frozen;
        }
    }

    //Row 6
    for (int i = 0; i < Q_STATE_N; i++)
    {
        if (i == 1 || i == 4 || i == 6)
        {
            grid[6][i] = Hole;
        }
        else
        {
            grid[6][i] = Frozen;
        }
    }

    //Row 7
    init_helper_1(grid[7], 3);
    grid[7][7] = Goal;
}
#endif

void print_grid()
{
    for (int i = 0; i < Q_STATE_N; i++)
    {
        for (int j = 0; j < Q_STATE_N; j++)
        {
            if ((curr_state / Q_STATE_N) == i && (curr_state % Q_STATE_N) == j)
                printf("*");

            switch (grid[i*Q_STATE_N + j])
            {
            case Start:
                printf("S ");
                break;
            case Frozen:
                printf("F ");
                break;
            case Hole:
                printf("H ");
                break;
            case Goal:
                printf("G ");
                break;
            default:
                break;
            }
        }
        printf("\n");
    }
}

void update_probability_matrix(struct pos *curr_pos, int action, struct ctx *new_ctx)
{
    struct pos new_pos;
    int new_state;
    int done = 0;
    int reward = 0;

    inc(curr_pos, action, &new_pos);
    new_state = to_s(&new_pos);
    done = (grid[new_pos.x * Q_STATE_N + new_pos.y] == Goal) || (grid[new_pos.x * Q_STATE_N + new_pos.y] == Hole);
    reward = (grid[new_pos.x * Q_STATE_N + new_pos.y] == Goal);

    new_ctx->done = done;
    new_ctx->reward = reward;
    new_ctx->new_state = new_state;
}

int to_s(struct pos *curr_pos)
{
    return curr_pos->x * Q_STATE_N + curr_pos->y;
}

void inc(struct pos *curr_pos, int action, struct pos *new_pos)
{

    new_pos->x = curr_pos->x;
    new_pos->y = curr_pos->y;

    switch (action)
    {
    case LEFT:
        new_pos->y = (curr_pos->y - 1 >= 0) ? curr_pos->y - 1 : 0;
        break;
    case RIGHT:
        new_pos->y = (curr_pos->y + 1 < Q_STATE_N) ? curr_pos->y + 1 : Q_STATE_N - 1;
        break;
    case UP:
        new_pos->x = (curr_pos->x - 1 >= 0) ? curr_pos->x - 1 : 0;
        break;
    case DOWN:
        new_pos->x = (curr_pos->x + 1 < Q_STATE_N) ? curr_pos->x + 1 : Q_STATE_N - 1;
        break;
    default:
        printf("Not an action!\n");
    }
}

void print_p_matrix()
{

    struct pos iter_pos;
    int s;

    for (int i = 0; i < Q_STATE_N; i++)
    {
        printf("-----------------------------------------------------------\n");
        for (int j = 0; j < Q_STATE_N; j++)
        {
            iter_pos.x = i;
            iter_pos.y = j;
            s = to_s(&iter_pos);
            printf("|");
            for (int k = 0; k < N_ACTION; k++)
            {
                // printf("(%f)", probability_matrix[s][k].p);

                printf("(%d)", probability_matrix[s][k].ctx.new_state);

                // printf("(%d)", probability_matrix[s][k].ctx.reward);

                // printf("(%d)", probability_matrix[s][k].ctx.done);
            }
        }
        printf("|");
        printf(" %d\n", i);
    }
        printf("-----------------------------------------------------------\n");
}

void env_setup()
{
    
#ifdef EIGHT
    initialize_grid_8();
#else
    initialize_grid_4();
#endif 

    int letter;
    struct pos iter_pos;
    int s;
    struct ctx iter_ctx;

    for (int row = 0; row < Q_STATE_N; row++)
    {
        for (int col = 0; col < Q_STATE_N; col++)
        {
            iter_pos.x = row;
            iter_pos.y = col;
            s = to_s(&iter_pos);

            for (int a = 0; a < N_ACTION; a++)
            {

                letter = grid[row*Q_STATE_N + col];

                if (letter == Goal || letter == Hole)
                {
                    probability_matrix[s][a].p = 1.0;
                    update_probability_matrix(&iter_pos, a, &iter_ctx);
                    probability_matrix[s][a].ctx.new_state = s;
                    probability_matrix[s][a].ctx.reward = 0;
                    probability_matrix[s][a].ctx.done = 1;
                }
                else
                {
                    probability_matrix[s][a].p = 1.0;
                    update_probability_matrix(&iter_pos, a, &iter_ctx);
                    probability_matrix[s][a].ctx.new_state = iter_ctx.new_state;
                    probability_matrix[s][a].ctx.reward = iter_ctx.reward;
                    probability_matrix[s][a].ctx.done = iter_ctx.done;
                }
            }
        }
    }
}

void env_reset()
{
    curr_state = 0;
}

void step(struct probability_matrix_item *m_item, int action)
{
    m_item->p = probability_matrix[curr_state][action].p;
    m_item->ctx.done = probability_matrix[curr_state][action].ctx.done;
    m_item->ctx.reward = probability_matrix[curr_state][action].ctx.reward;
    m_item->ctx.new_state = probability_matrix[curr_state][action].ctx.new_state;

    curr_state = m_item->ctx.new_state;
    last_action = action;

    //Do a dummy read to simulate writing to peripheral register
    dummy_peripheral[0] = 10; 

#ifdef DEBUG
    switch (action)
    {
    case LEFT:
        printf("Left\n");
        break;
    case RIGHT:
        printf("Right\n");
        break;
    case UP:
        printf("Up\n");
        break;
    case DOWN:
        printf("Down\n");
        break;
    default:
        printf("Illegal action\n");
    }

    print_grid();
    printf("\n");
#endif
}

void EAPP_ENTRY eapp_entry(int AGENT_TID){
    printf("Enter Simulator\n");


    for(int i = 0; i < 1000; i++){
        syscall_task_yield();
    }

    printf("Simulator End\n");

    syscall_task_return();

    env_setup();
    struct send_action_args args;
    struct probability_matrix_item p_item;
    int reset_ack = 1; 

    while(1){

        while(sbi_recv(AGENT_TID, &args, sizeof(struct send_action_args), YIELD));
        
        switch(args.msg_type){
            case RESET:
                env_reset();
                sbi_send(AGENT_TID, &reset_ack, sizeof(int), YIELD);
                break;
            case STEP:
                step(&p_item, args.action);
                sbi_send(AGENT_TID, &p_item.ctx, sizeof(struct ctx), YIELD); 
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
    syscall_task_return();
}