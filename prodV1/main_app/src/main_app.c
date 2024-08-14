/*
 * main_app.c
 *
 *  Created on: Aug 6, 2024
 *      Author: adith
 */
/*
 * main_app.c
 *
 *  Created on: Aug 6, 2024
 *      Author: adith
 */

#include "main_app.h"


// Defining state machine handle
StateMachine_t StateMachine;



void main_app(void)
{
    // Initialize the state machine
    StateMachine.eState = ST_INIT;
    StateMachine.eEvent = NO_EVENT;
    StateMachine.bEventOccurred = false;

    // Main loop
    while (1)
    {
        // Run the state machine
        StateTransition();

        // Other system tasks can be performed here
    }
}
