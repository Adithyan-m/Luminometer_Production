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


HAL_StatusTypeDef HMI_changepage(UART_HandleTypeDef *uart, uint16_t pagenum){

	uint8_t buffer[10];  // Buffer to hold all bytes to be sent

    // Prepare the buffer
    buffer[0] = 0x5A;
    buffer[1] = 0xA5;
    buffer[2] = 0x07;
    buffer[3] = 0x82;
    buffer[4] = 0x00;
    buffer[5] = 0x84;
    buffer[6] = 0x5A;
    buffer[7] = 0x01;

    // Add PAGE_MENU as two bytes (16 bits)
    buffer[8] = (pagenum >> 8) & 0xFF;  // High byte
    buffer[9] = pagenum & 0xFF;         // Low byte

    // Changes page to menu
    HAL_UART_Transmit(uart, buffer, 10 , HAL_MAX_DELAY);

    return HAL_OK;
}
