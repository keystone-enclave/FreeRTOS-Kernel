#ifndef _RL_CONFIG_H

/* *** Configurations  *** */

/* Enables all tasks to be switched via the SM*/

/* All tasks must be registered by the SM */
// #define TASK_REGISTER_ALL

/* Enclave Agent <-> Enclave Driver*/
// #define EA_ED_RL

/* Task Agent <-> Task Driver*/
// #define TA_TD_RL

/* Enclave Agent <-> Task Driver*/
// #define EA_TD_RL

/* Task Agent <-> Enclave Driver*/
// #define TA_ED_RL

// #define RV8

// #define MSG_TEST_TASK
#define MSG_TEST_ENCLAVE


#ifdef EA_ED_RL
    #define AGENT_TID 1
    #define DRIVER_TID 2
#endif

#ifdef TA_TD_RL
/* We do nothing since tasks do not need to use the mailbox. */
#endif

#ifdef EA_TD_RL
    #ifdef TASK_REGISTER_ALL
    #define AGENT_TID 1
    #define DRIVER_TID 2
    #else
    #define AGENT_TID 1
    #define DRIVER_TID 0
    #endif
#endif

#ifdef TA_ED_RL
    #ifdef TASK_REGISTER_ALL
    #define AGENT_TID 1
    #define DRIVER_TID 2
    #else
    #define AGENT_TID 0
    #define DRIVER_TID 1
    #endif
#endif

/* USED FOR DEBUGGING PURPOSES! */

/* Runs purely enclave task tests. */
// #define ENCLAVE

/* Runs purely task tests */
// #define TEST



#endif