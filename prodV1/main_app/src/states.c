/*
 * states.c
 *
 * Created on: Aug 7, 2024
 * Author: adith
 */

#include "states.h"

// Include necessary headers for MX_* functions
// For example:
// #include "fatfs.h"
// #include "usb_host.h"

void StateTransition(void)
{
    if (StateMachine.bEventOccurred)
    {
        StateMachine.bEventOccurred = false;
        for (uint32_t idx = 0U; idx < STATE_TABLE_SIZE; idx++)
        {
            if ((StateTransitionTable[idx].eCurrState == StateMachine.eState) &&
                (StateTransitionTable[idx].eEvent == StateMachine.eEvent))
            {
                StateMachine.eState = StateTransitionTable[idx].eNextState;
                break;
            }
        }
    }
    StateArray[StateMachine.eState]();
}

void stateInit(void) {

    HAL_Delay(2000);

    // Send command to change screen to system check screen
    StateMachine.bEventOccurred = true;
    StateMachine.eEvent = EVENT_INIT_COMPLETE;

    HAL_StatusTypeDef status = initLevelSensors(&hi2c1);
    if (status != HAL_OK) {
        Error_Handler();
    }

    // Exit the state function (the event will be handled in the state transition function)
    return;
}

void stateStartUp(void) {

    // Implement startup logic here
	// Some kind of handshake, for now assume it works

	//uint32_t data
	//ReadData(DATA_TYPE_ERROR_CODES, 1, &data, sizeof(data));

	// Homming cuvettte holder
	if(HAL_GPIO_ReadPin(LIMIT_SW_1_GPIO_Port, LIMIT_SW_1_Pin) == GPIO_PIN_SET){
		cuvetteRotate(&htim9, &hdac);
	}

	// Dark count
	uint32_t new_dark_count = 0000;
	new_dark_count = PULSES_darkcount(&htim2);

	if (new_dark_count > DARK_THRESH){
	    StateMachine.bEventOccurred = true;
	    StateMachine.eEvent = EVENT_STARTUP_FAILED_DARK;
        return;
	}

	// Saving new dark count
	HAL_StatusTypeDef status = WriteDarkCount(new_dark_count);
	if (status != HAL_OK) {
	    // Handle error
	    StateMachine.bEventOccurred = true;
	    StateMachine.eEvent = EVENT_STARTUP_FAILED_FLASH;
        return;
	}

	// Starter Level
    if(calculate_level(&hi2c1) < 5 ){
        StateMachine.bEventOccurred = true;
        StateMachine.eState = EVENT_STARTUP_FAILED_STARTER;
        return;
    }


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
    buffer[8] = (PAGE_MENU >> 8) & 0xFF;  // High byte
    buffer[9] = PAGE_MENU & 0xFF;         // Low byte

    // Changes page to menu
    HAL_UART_Transmit(&huart2, buffer, 10 , HAL_MAX_DELAY);
    // Send 5A A5 07 82 00 84 5A 01 PAGE_MENU

    StateMachine.bEventOccurred = true;
    StateMachine.eEvent = EVENT_STARTUP_COMPLETE;

    return;



}

void stateHomeMenu(void) {
    // Implement home menu logic here
}

void stateError(void) {
    // Implement error handling logic here
}
