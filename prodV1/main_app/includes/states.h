/*
 * states.h
 *
 * Created on: Aug 7, 2024
 * Author: adith
 */

#ifndef MAIN_APP_INCLUDES_STATES_H_
#define MAIN_APP_INCLUDES_STATES_H_

#include "main_app.h"
#include <stdbool.h>

// Forward declarations
void stateInit(void);
void stateStartUp(void);
void stateHomeMenu(void);
void stateSystemCheck(void);
void stateError(void);

// State machine states
typedef enum {
    ST_INIT,
    ST_STARTUP,
    ST_HOMEMENU,
    ST_MEASURE,
    ST_TEST_REPORT,
    ST_QC_REPORT,
    ST_SETTINGS,
    ST_SYSTEM_CHECK,
    ST_SERVICES,
    ST_ERROR,
    ST_MAX_STATE
} MACHINE_STATE_t;

// State machine trigger events
typedef enum {
    NO_EVENT = 0,
    EVENT_INIT_COMPLETE,
    
    EVENT_STARTUP_COMPLETE,
    EVENT_STARTUP_FAILED_DARK,
    EVENT_STARTUP_FAILED_HANDSHAKE,
    EVENT_STARTUP_FAILED_STARTER,
	EVENT_STARTUP_FAILED_FLASH,
    
    EVENT_MENU_MEASURE,
    EVENT_MENU_TEST_REPORT,
    EVENT_MENU_QC_REPORT,
    EVENT_MENU_SETTINGS,
    EVENT_MENU_SYSTEM_CHECK,
    EVENT_MENU_SERVICES,
    
    EVENT_BACK,
    EVENT_HMI_FALIURE,

    EVENT_SYSCEHCK_CUV_FAILED,
    EVENT_SYSCHECK_PRIME_FAILED,
    
	EVENT_ERROR_IGNORE
} MACHINE_EVENT_t;

// Function pointer type for state functions
typedef void (*StateFunctionPtr)(void);

// State array for function pointers
static const StateFunctionPtr StateArray[ST_MAX_STATE] = {
    stateInit,
    stateStartUp,
    stateHomeMenu,
    stateSystemCheck,
    stateError
};

// State transition struct
typedef struct {
    MACHINE_STATE_t eCurrState;
    MACHINE_EVENT_t eEvent;
    MACHINE_STATE_t eNextState;
} STATE_TRANSITION_t;

// Transition table
static const STATE_TRANSITION_t StateTransitionTable[] = {

    /* Current State    Event                           Next State */
    { ST_INIT,          NO_EVENT,                       ST_INIT },
    { ST_INIT,          EVENT_INIT_COMPLETE,            ST_STARTUP },

    { ST_STARTUP,       NO_EVENT,             			ST_STARTUP },
    { ST_STARTUP,       EVENT_STARTUP_COMPLETE, 		ST_HOMEMENU },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_DARK, 		ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_HANDSHAKE, ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_STARTER, 	ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_FLASH, 	ST_ERROR },

    { ST_HOMEMENU,      NO_EVENT,                       ST_HOMEMENU },
    { ST_HOMEMENU,      EVENT_MENU_MEASURE,             ST_MEASURE },
    { ST_HOMEMENU,      EVENT_MENU_TEST_REPORT,         ST_TEST_REPORT },
    { ST_HOMEMENU,      EVENT_MENU_QC_REPORT,           ST_QC_REPORT },
    { ST_HOMEMENU,      EVENT_MENU_SETTINGS,            ST_SETTINGS },
    { ST_HOMEMENU,      EVENT_MENU_SYSTEM_CHECK,        ST_SYSTEM_CHECK },
    { ST_HOMEMENU,      EVENT_MENU_SERVICES,            ST_SERVICES },
    { ST_HOMEMENU,      EVENT_HMI_FALIURE,              ST_SERVICES },

    { ST_SYSTEM_CHECK,  NO_EVENT,                       ST_SYSTEM_CHECK },
    { ST_SYSTEM_CHECK,  EVENT_SYSCEHCK_CUV_FAILED,      ST_ERROR },
    { ST_SYSTEM_CHECK,  EVENT_SYSCHECK_PRIME_FAILED,    ST_ERROR },
    { ST_SYSTEM_CHECK,  EVENT_HMI_FALIURE,              ST_ERROR },


    { ST_ERROR,         NO_EVENT,                       ST_ERROR },
    { ST_ERROR,         EVENT_ERROR_IGNORE,             ST_HOMEMENU }
};

// Number of transitions in the StateTransitionTable array
#define STATE_TABLE_SIZE (sizeof(StateTransitionTable) / sizeof(StateTransitionTable[0]))

// State Machine struct
typedef struct {
    MACHINE_STATE_t eState;
    MACHINE_EVENT_t eEvent;
    bool bEventOccurred;
} StateMachine_t;

// Global State Machine instance
extern StateMachine_t StateMachine;

// State transition function
void StateTransition(void);

#endif /* MAIN_APP_INCLUDES_STATES_H_ */
