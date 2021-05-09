
#ifndef _SIM_H

#define _SIM_H

#ifdef EIGHT
#define Q_STATE_N 8
#else
#define Q_STATE_N 4
#endif

#define Q_STATE Q_STATE_N *Q_STATE_N
#define N_ACTION 4

#define YIELD 0

#define NUM_EPISODES 1000
#define STEPS_PER_EP 1000

#define LEARN_RATE 0.5
#define DISCOUNT 0.95

#define TRUE 1
#define FALSE 0

#define E_GREEDY 0.7
#define E_GREEDY_F 0.01
#define E_GREEDY_DECAY 0.99

enum tile_state
{
    Start,
    Frozen,
    Hole,
    Goal
};
enum action
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NO_ACTION
};

enum msg_type{
    RESET,
    STEP,
    FINISH
};

// "SFFFFFFF", 0
// "FFFFFFFF", 1
// "FFFHFFFF", 2
// "FFFFFHFF", 3
// "FFFHFFFF", 4
// "FHHFFFHF", 5
// "FHFFHFHF", 6
// "FFFHFFFG"  7

// "SFFF",
// "FHFH",
// "FFFH",
// "HFFG"


struct pos
{
    int x;
    int y;
};

struct ctx
{
    int new_state;
    int reward;
    int done;
};

struct probability_matrix_item
{
    double p;
    struct ctx ctx;
};

struct send_action_args {
    int action; 
    int msg_type; 
};


int to_s(struct pos *curr_pos);
void inc(struct pos *curr_pos, int action, struct pos *new_pos);
void update_probability_matrix(struct pos *curr_pos, int action, struct ctx *new_ctx);
double getReward(struct pos pos, int act);
void step(struct probability_matrix_item *m_item, int action);
void env_setup();
void env_reset();
void print_p_matrix();

#endif