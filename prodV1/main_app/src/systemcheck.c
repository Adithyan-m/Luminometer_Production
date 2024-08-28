/*
 * systemcheck.c
 *
 *  Created on: Aug 24, 2024
 *      Author: adith
 */

// TODO: Implement back button
// TODO: Add disabling back button

#include "systemcheck.h"
#include "states.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
#define STRING_SIZE 100

void autoCheck(void) {

    uint8_t databuffer[BUFFER_SIZE] = {0};

    // Switch page to "place cuvette and press okay"
    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "place cuvette and press continue");
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }
    HMI_eraseString(&huart2);

    if(databuffer[9] == KEY_BACK){
        // return back to sys check menu
        HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);
        return;
    }
    else if(databuffer[9] != KEY_YES) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    // TODO: add correct timer handle
    cuvetteRotate(&htim1);

    // Background check
    uint32_t newBackground = PULSES_background(&htim2);

    // Displaying on screen
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "the background is %lu units", newBackground);
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    // Write background value to memory
    WriteBackground(newBackground);
    
    HMI_eraseString(&huart2);

    // Wait to erase screen 
    HAL_Delay(3000);
    HMI_eraseString(&huart2);

    // Press start priming pump
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue to do priming");
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    // Disable yes no button
    bool primed = false;
    for (int i = 0; i < 2; i++){
        // loop for primimng the pump
        
        // wait for key press
        while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }
        
        if(databuffer[9] == KEY_BACK) {
            // return back to sys check menu
            HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);
            return;

        } else if(databuffer[9] != KEY_YES){
            StateMachine.bEventOccurred = true;
            StateMachine.eEvent = EVENT_HMI_FALIURE;
            return;  
        }

        // TODO : correct handle
        cuvetteRotate(&htim2, &hadc1);
        primePump(&htim2);
        cuvetteRotate(&htim2, &hadc1);
        
        // Check for user input
        HMI_eraseString(&huart1);
        stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Is there liquid in the cuvette");
        HMI_writeString(&huart2, 2100, stringToSend);
        free(stringToSend);

        // wait for key press
        while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        } 

        if(databuffer[9] == KEY_YES) {
            primed = true;
            break;
        }

    }

    if(!primed) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCHECK_PRIME_FAILED;
        return;  
    }

    HMI_eraseString(&huart2);

    // Change page to cotinue page

    // Check for user input
    HMI_eraseString(&huart1);
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue for light check");
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    // Press start Light Check
    while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != 0xAA) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(&htim1, 20, 20);
    newBackground = backgroundCheck(&htim1);
   
   // Displaying on screen
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "the lightcheck is %lu units", newBackground);
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    WriteLightCheck(newBackground);

    HAL_Delay(3000);
    HMI_eraseString(&huart1);
    HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);

    return;

}   

void backgroundCheck(void) {
    // Perform background check
    uint32_t background = PULSES_background(&HANDLE_PULSES);

    // Display result
    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Background: %lu units", background);
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    // Write background value to memory
    WriteBackground(background);

    HAL_Delay(3000);
    HMI_eraseString(&huart2);

    return ;
}

void pumpPriming(void) {
    
    
    uint8_t databuffer[BUFFER_SIZE] = {0};
    bool primed = false;

    for (int i = 0; i < 2; i++) {
        char *stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Priming attempt %d. Press continue.", i + 1);
        HMI_writeString(&huart2, 2100, stringToSend);
        free(stringToSend);

        while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }

        if(databuffer[9] != 0xAA) {
            break;
        }

        cuvetteRotate(htim);
        primePump(htim);
        cuvetteRotate(htim);

        HMI_eraseString(&huart2);
        stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Is there liquid in the cuvette?");
        HMI_writeString(&huart2, 2100, stringToSend);
        free(stringToSend);

        while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }

        if(databuffer[9] == 0xAA) {
            primed = true;
            break;
        }
    }

    if (!primed) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCHECK_PRIME_FAILED;
    }
}

void darkCheck(TIM_HandleTypeDef *htim) {

    uint32_t darkReading = PULSES_background(htim);

    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Dark reading: %lu units", darkReading);
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    WriteDarkCount(darkReading);

    HAL_Delay(3000);
    HMI_eraseString(&huart2);
}

void lightCheck(TIM_HandleTypeDef *htim) {

    uint8_t databuffer[BUFFER_SIZE] = {0};

    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue for light check");
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    while(HAL_UART_Receive(&huart2, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != 0xAA) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(htim, 20, 20);
    uint32_t lightReading = PULSES_background(htim);

    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Light check: %lu units", lightReading);
    HMI_writeString(&huart2, 2100, stringToSend);
    free(stringToSend);

    WriteLightCheck(lightReading);

    HAL_Delay(3000);
    HMI_eraseString(&huart2);
}
