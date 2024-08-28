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

    // Comment out
	if (new_dark_count > DARK_THRESH){
	    StateMachine.bEventOccurred = true;
	    StateMachine.eEvent = EVENT_STARTUP_FAILED_DARK;
       return;
	}

	// Saving new dark count
	HAL_StatusTypeDef status = WriteDarkCount(new_dark_count);

    // Comment out
	if (status != HAL_OK) {
	    // Handle error
	    StateMachine.bEventOccurred = true;
	    StateMachine.eEvent = EVENT_STARTUP_FAILED_FLASH;
       return;
	}

	// Starter Level
    // Comment out
    if(calculate_level(&hi2c1) < 5 ){
       StateMachine.bEventOccurred = true;
       StateMachine.eState = EVENT_STARTUP_FAILED_STARTER;
       return;
    }


    StateMachine.bEventOccurred = true;
    StateMachine.eEvent = EVENT_STARTUP_COMPLETE;

    HAL_Delay(2000);
    HMI_changepage(&huart2, PAGE_MENU);

    return;

}

void stateHomeMenu(void) {
    
    HMI_changepage(&huart2, PAGE_MENU);  // Changing page to menu
    
    // Initializing Data Buffer
    uint8_t dataBuffer[10] = {0};

    // Wait till a valid keypress to avoid random messages
    while(1){

        // Wait till we recieve 10 bytes of data (1 key press)
        if(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY == HAL_OK)){
            
            // Check if it is for a key press
            if(dataBuffer[8] == 0xFF){
                
                // Switch on keypress and assign right state then return
                switch (dataBuffer[9]){
                    
                    case 0x00:
                        // Measure Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_MEASURE;                       
                        break;
                    
                    case 0x01:
                        // Test Reports Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_TEST_REPORT;  
                        break;

                    case 0x02:
                        // QC Reports Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_QC_REPORT;  
                        break;

                    case 0x03:
                        // Settings Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_SETTINGS;  
                        break;
                    
                    case 0x04:
                        // System Check Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_SYSTEM_CHECK;  
                        break;

                    case 0x05:
                        // Services Menu
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_MENU_SERVICES;  
                        break;

                    default:
                        break;
                }

                return;
            }
        }
    }
}


void stateSystemCheck(void){
    
    // Wait till a valid keypress to avoid random messages
    uint8_t dataBuffer[10] = {0};

    while(1){

        // Wait till we recieve 10 bytes of data (1 key press)
        if(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY == HAL_OK)){
            
            // Check if it is for a key press
            if(dataBuffer[8] == 0xFF){
                
                // Switch on keypress and assign right state then return
                switch (dataBuffer[9]){
                    
                    case 0x40:
                        // Auto Check                      
                        autoCheck(void);
                        break;
                    
                    case 0x41:
                        // Background Check
                        backgroundCheck(void);
                        break;

                    case 0x42:
                        // Pump Primimg
                        pumpPriming(void);
                        break;

                    case 0x43:
                        // Dark Check
                        darkCheck(void);
                        break;
                    
                    case 0x44:
                        // Light Check
                        lightCheck(void);
                        break;

                    case KEY_BACK:
                        // back button
                        StateMachine.bEventOccurred = true;
                        StateMachine.eEvent = EVENT_BACK; 
                        return;

                    default:
                        break;
                }
            }
        }
    }

}

void stateError(void) {
    // Implement error handling logic here
}
