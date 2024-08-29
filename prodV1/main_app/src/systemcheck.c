/*
 * systemcheck.c
 *
 *  Created on: Aug 24, 2024
 *      Author: adith
 */

// TODO: Implement back button
// TODO: Add disabling back button

#include "systemcheck.h"


#define BUFFER_SIZE 10
#define STRING_SIZE 100

void autoCheck(void) {

    uint8_t databuffer[BUFFER_SIZE] = {0};

    // Switch page to "place cuvette and press okay"
    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "place cuvette and press continue");
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }
    HMI_eraseString(&HANDLE_HMI,0x2100);

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
    cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);

    // Background check
    uint32_t newBackground = PULSES_background(&HANDLE_PULSES);

    // Displaying on screen
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "the background is %lu units", newBackground);
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    // Write background value to memory
    WriteBackground(newBackground);
    
    HMI_eraseString(&HANDLE_HMI,0x2100);

    // Wait to erase screen 
    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI,0x2100);

    // Press start priming pump
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue to do priming");
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    // Disable yes no button
    bool primed = false;
    for (int i = 0; i < 2; i++){
        // loop for primimng the pump
        
        // wait for key press
        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
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
        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        primePump(&HANDLE_PUMP1, &HANDLE_PUMP2, &HANDLE_DAC);
        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        
        // Check for user input
        HMI_eraseString(&HANDLE_HMI, 0x2100);
        stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Is there liquid in the cuvette");
        HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
        free(stringToSend);

        // wait for key press
        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
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

    HMI_eraseString(&HANDLE_HMI,0x2100);

    // Change page to cotinue page

    // Check for user input
    HMI_eraseString(&HANDLE_HMI, 0x2100);
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue for light check");
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    // Press start Light Check
    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != 0xAA) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(&HANDLE_PUMP1, &HANDLE_DAC, &HANDLE_PUMP2, 20, 20);
    newBackground = PULSES_background(&htim1);
   
   // Displaying on screen
    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "the lightcheck is %lu units", newBackground);
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    WriteLightCheck(newBackground);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, 0x2100);
    HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);

    return;

}   

void backgroundCheck(void) {
    // Perform background check
    uint32_t background = PULSES_background(&HANDLE_PULSES);

    // Display result
    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Background: %lu units", background);
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    // Write background value to memory
    WriteBackground(background);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI,0x2100);

    return ;
}

void pumpPriming(void) {
    
    
    uint8_t databuffer[BUFFER_SIZE] = {0};
    bool primed = false;

    for (int i = 0; i < 2; i++) {
        char *stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Priming attempt %d. Press continue.", i + 1);
        HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
        free(stringToSend);

        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }

        if(databuffer[9] != 0xAA) {
            break;
        }

        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        primePump(&HANDLE_PUMP1, &HANDLE_PUMP2, &HANDLE_DAC);
        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);

        HMI_eraseString(&HANDLE_HMI,0x2100);
        stringToSend = malloc(STRING_SIZE * sizeof(char));
        sprintf(stringToSend, "Is there liquid in the cuvette?");
        HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
        free(stringToSend);

        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
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

void darkCheck(void) {

    uint32_t darkReading = PULSES_background(&HANDLE_PULSES);

    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Dark reading: %lu units", darkReading);
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    WriteDarkCount(darkReading);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI,0x2100);
}

void lightCheck(void) {

    uint8_t databuffer[BUFFER_SIZE] = {0};

    char *stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Press continue for light check");
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != 0xAA) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(&HANDLE_PUMP1,&HANDLE_DAC,&HANDLE_PUMP2, 20, 20);
    uint32_t lightReading = PULSES_background(&HANDLE_PULSES);

    stringToSend = malloc(STRING_SIZE * sizeof(char));
    sprintf(stringToSend, "Light check: %lu units", lightReading);
    HMI_writeString(&HANDLE_HMI, 0x2100, stringToSend);
    free(stringToSend);

    WriteLightCheck(lightReading);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI,0x2100);
}
