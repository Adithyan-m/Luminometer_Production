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
void stateError(void);

// State machine states
typedef enum {
    ST_INIT,
    ST_STARTUP,
    ST_HOMEMENU,
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
	EVENT_ERROR_IGNORE
} MACHINE_EVENT_t;

// Function pointer type for state functions
typedef void (*StateFunctionPtr)(void);

// State array for function pointers
static const StateFunctionPtr StateArray[ST_MAX_STATE] = {
    stateInit,
    stateStartUp,
    stateHomeMenu,
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
    /* Current State    Event                 Next State */
    { ST_INIT,          NO_EVENT,             ST_INIT },
    { ST_INIT,          EVENT_INIT_COMPLETE,  ST_STARTUP },

    { ST_STARTUP,       NO_EVENT,             			ST_STARTUP },
    { ST_STARTUP,       EVENT_STARTUP_COMPLETE, 		ST_HOMEMENU },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_DARK, 		ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_HANDSHAKE, ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_STARTER, 	ST_ERROR },
    { ST_STARTUP,       EVENT_STARTUP_FAILED_FLASH, 	ST_ERROR },

    { ST_HOMEMENU,      NO_EVENT,             ST_HOMEMENU },

    { ST_ERROR,         NO_EVENT,             ST_ERROR },
    { ST_ERROR,         EVENT_ERROR_IGNORE,   ST_HOMEMENU }
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
