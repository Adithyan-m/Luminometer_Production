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



HAL_StatusTypeDef HMI_writeString(UART_HandleTypeDef *uart, uint16_t vpAddr, const char* string, size_t length) {
    uint16_t totalLength = length + 6; // 3 bytes for the header (excluding 5A A5)

    // Check if the string is too long for the protocol or our buffer
    if (totalLength > 0xFFFF || totalLength + 6 > MAX_HMI_MESSAGE_SIZE) {
        return HAL_ERROR;
    }

    // Prepare the message
    hmiBuffer[0] = 0x5A;
    hmiBuffer[1] = 0xA5;
    hmiBuffer[2] = length + 4;
    hmiBuffer[3] = 0x82;
    hmiBuffer[4] = 0x00FF & (vpAddr >> 8);
    hmiBuffer[5] = 0x00FF & vpAddr;

    // Copy the string to the buffer
    memcpy(hmiBuffer + 6, string, length);

    // Transmit the entire message
    return HAL_UART_Transmit(uart, hmiBuffer, totalLength + 6, HAL_MAX_DELAY);
}

void writeToHMI(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int length = vsnprintf(stringBuffer, STRING_SIZE, format, args);
    va_end(args);

    if (length > 0 && length < STRING_SIZE) {
        HMI_writeString(&HANDLE_HMI, HMI_STRING_ADDRESS, stringBuffer, length);
    } else if (length >= STRING_SIZE) {
        // Message was truncated
        HMI_writeString(&HANDLE_HMI, HMI_STRING_ADDRESS, "Error: Message too long", 23);
    } else {
        // Encoding error occurred
        HMI_writeString(&HANDLE_HMI, HMI_STRING_ADDRESS, "Error: Invalid message", 22);
    }
}

HAL_StatusTypeDef HMI_eraseString(UART_HandleTypeDef *uart, uint16_t vpAddr){

    uint8_t buffer[6] = {0};
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
 
}
