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
    HMI_changepage(&huart2, PAGE_SPASH);

    HAL_Delay(2000);

    // Send command to change screen to system check screen

    HAL_StatusTypeDef status = initLevelSensors(&hi2c1);
    if (status != HAL_OK) {
       // Error_Handler();
    }

    StateMachine.bEventOccurred = true;
    StateMachine.eEvent = EVENT_INIT_COMPLETE;

    HMI_changepage(&huart2, PAGE_INIT);

    // Exit the state function (the event will be handled in the state transition function)
    return;
}

void stateStartUp(void) {

    // Implement startup logic here
	// Some kind of handshake, for now assume it works

	//uint32_t data
	//ReadData(DATA_TYPE_ERROR_CODES, 1, &data, sizeof(data));

	// Bringing cuvette holder to position TODO: Change it to RESET
	if(HAL_GPIO_ReadPin(LIMIT_SW_1_GPIO_Port, LIMIT_SW_1_Pin) == GPIO_PIN_RESET){
		cuvetteRotate(&htim9, &hdac);
	}

	// Dark count
	uint32_t new_dark_count = 0000;
	new_dark_count = PULSES_darkcount(&htim2);

//	if (new_dark_count > DARK_THRESH){
//	    StateMachine.bEventOccurred = true;
//	    StateMachine.eEvent = EVENT_STARTUP_FAILED_DARK;
//        return;
//	}

	// Saving new dark count
	HAL_StatusTypeDef status = WriteDarkCount(new_dark_count);
//	if (status != HAL_OK) {
//	    // Handle error
//	    StateMachine.bEventOccurred = true;
//	    StateMachine.eEvent = EVENT_STARTUP_FAILED_FLASH;
//        return;
//	}

	// Starter Level
    if(calculate_level(&hi2c1) < 5 ){
//        StateMachine.bEventOccurred = true;
//        StateMachine.eState = EVENT_STARTUP_FAILED_STARTER;
//        return;
    }


    StateMachine.bEventOccurred = true;
    StateMachine.eEvent = EVENT_STARTUP_COMPLETE;

    HMI_changepage(&huart2, PAGE_MENU);
    HAL_Delay(2000);

    return;

}

void stateHomeMenu(void) {
    // Implement home menu logic here
}

void stateError(void) {
    // Implement error handling logic here
}
