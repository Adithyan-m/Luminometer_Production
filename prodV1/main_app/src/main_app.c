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



HAL_StatusTypeDef HMI_writeString(UART_HandleTypeDef *uart, uint16_t vpAddr, const char* string) {

    uint8_t *clearBuffer = (uint8_t*)malloc(6);
    clearBuffer[0] = 0x5A;
    clearBuffer[1] = 0xA5;
    clearBuffer[2] = 0X03;
    clearBuffer[3] = 0x82;
    clearBuffer[4] = 0x4F;
    clearBuffer[5] = 0x4B;

    uint16_t stringLength = strlen(string);
    uint16_t totalLength = stringLength + 6; // 3 bytes for the header (excluding 5A A5)

    // Check if the string is too long for the protocol
    if (totalLength > 0xFFFF) {
        return HAL_ERROR;
    }

    // Allocate a buffer for the entire message
    uint8_t* buffer = (uint8_t*)malloc(totalLength + 6); // +4 for the 5A A5 size 82 VV PP
    if (buffer == NULL) {
        return HAL_ERROR;
    }

    // Prepare the message
    buffer[0] = 0x5A;
    buffer[1] = 0xA5;
    buffer[2] = stringLength + 4;
    buffer[3] = 0x82;
    buffer[4] = 0x00FF & (vpAddr >> 8);
    buffer[5] = 0x00FF & vpAddr;

    // Copy the string to the buffer
    memcpy(buffer + 6, string, stringLength);

    // Transmit the entire message
    HAL_StatusTypeDef status = HAL_UART_Transmit(uart, buffer, totalLength + 6, HAL_MAX_DELAY);

    // Transmit the clear buffer
    HAL_StatusTypeDef clearStatus = HAL_UART_Transmit(uart, clearBuffer, 6, HAL_MAX_DELAY);

    // Free the allocated buffer
    free(buffer);
    free(clearBuffer);

    // Return the status of the first transmission, or the clear transmission if the first was successful
    return (status == HAL_OK) ? clearStatus : status;
}

HAL_StatusTypeDef HMI_eraseString(UART_HandleTypeDef *uart, uint16_t vpAddr){

    uint8_t buffer[6] = {0};
    uint8_t *clearBuffer  = (uint8_t*)malloc(6);
    clearBuffer[0] = 0x5A;
    clearBuffer[1] = 0xA5;
    clearBuffer[2] = 0X03;
    clearBuffer[3] = 0x82;
    clearBuffer[4] = 0x4F;
    clearBuffer[5] = 0x4B;
    // Prepare the message
    buffer[0] = 0x5A;
    buffer[1] = 0xA5;
    buffer[2] = 0x00;
    buffer[3] = 0x82;
    buffer[4] = 0x00FF & (vpAddr >> 8);
    buffer[5] = 0x00FF & vpAddr;

        // Transmit the entire message
    HAL_UART_Transmit(uart, buffer, 6, HAL_MAX_DELAY);
    HAL_UART_Transmit(uart, clearBuffer, 6, HAL_MAX_DELAY);

    free(clearBuffer);

}
