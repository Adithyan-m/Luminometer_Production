#include "systemcheck.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_SIZE 10
#define STRING_SIZE 300
#define MAX_HMI_MESSAGE_SIZE 256

// Define constants for magic numbers
#define HMI_STRING_ADDRESS 0x2100
#define KEY_YES 0xAA
#define KEY_BACK 0xBB

static char stringBuffer[STRING_SIZE];
static uint8_t hmiBuffer[MAX_HMI_MESSAGE_SIZE];

// writeToHMI and HMI_writeString functions are omitted as requested

void autoCheck(void) {
    uint8_t databuffer[BUFFER_SIZE] = {0};

    writeToHMI("place cuvette and press continue");

    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);

    if(databuffer[9] == KEY_BACK) {
        HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);
        return;
    }
    else if(databuffer[9] != KEY_YES) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);

    uint32_t newBackground = PULSES_background(&HANDLE_PULSES);

    writeToHMI("the background is %lu units", newBackground);

    WriteBackground(newBackground);
    
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);

    writeToHMI("Press continue to do priming");

    bool primed = false;
    for (int i = 0; i < 2 && !primed; i++) {
        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }
        
        if(databuffer[9] == KEY_BACK) {
            HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);
            return;
        } else if(databuffer[9] != KEY_YES) {
            StateMachine.bEventOccurred = true;
            StateMachine.eEvent = EVENT_HMI_FALIURE;
            return;  
        }

        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        primePump(&HANDLE_PUMP1, &HANDLE_PUMP2, &HANDLE_DAC);
        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        
        HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
        writeToHMI("Is there liquid in the cuvette");

        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        } 

        if(databuffer[9] == KEY_YES) {
            primed = true;
        }
    }

    if(!primed) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCHECK_PRIME_FAILED;
        return;  
    }

    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);

    writeToHMI("Press continue for light check");

    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != KEY_YES) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(&HANDLE_PUMP1, &HANDLE_DAC, &HANDLE_PUMP2, 20, 20);
    newBackground = PULSES_background(&HANDLE_PULSES);
   
    writeToHMI("the lightcheck is %lu units", newBackground);

    WriteLightCheck(newBackground);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
    HMI_changepage(&huart1, PAGE_SYSTEMS_CHECK);
}   

void backgroundCheck(void) {
    uint32_t background = PULSES_background(&HANDLE_PULSES);

    writeToHMI("Background: %lu units", background);

    WriteBackground(background);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
}

void pumpPriming(void) {
    uint8_t databuffer[BUFFER_SIZE] = {0};
    bool primed = false;

    for (int i = 0; i < 2 && !primed; i++) {
        writeToHMI("Priming attempt %d. Press continue.", i + 1);

        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }

        if(databuffer[9] != KEY_YES) {
            break;
        }

        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);
        primePump(&HANDLE_PUMP1, &HANDLE_PUMP2, &HANDLE_DAC);
        cuvetteRotate(&HANDLE_CUVETTE, &HANDLE_DAC);

        HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
        writeToHMI("Is there liquid in the cuvette?");

        while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
            __NOP();
        }

        if(databuffer[9] == KEY_YES) {
            primed = true;
        }
    }

    if (!primed) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCHECK_PRIME_FAILED;
    }
}

void darkCheck(void) {
    uint32_t darkReading = PULSES_background(&HANDLE_PULSES);

    writeToHMI("Dark reading: %lu units", darkReading);

    WriteDarkCount(darkReading);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
}

void lightCheck(void) {
    uint8_t databuffer[BUFFER_SIZE] = {0};

    writeToHMI("Press continue for light check");

    while(HAL_UART_Receive(&HANDLE_HMI, databuffer, BUFFER_SIZE, HAL_MAX_DELAY) != HAL_OK) {
        __NOP();
    }

    if(databuffer[9] != KEY_YES) {
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_HMI_FALIURE;
        return;  
    }

    dispenseStarterSimul(&HANDLE_PUMP1, &HANDLE_DAC, &HANDLE_PUMP2, 20, 20);
    uint32_t lightReading = PULSES_background(&HANDLE_PULSES);

    writeToHMI("Light check: %lu units", lightReading);

    WriteLightCheck(lightReading);

    HAL_Delay(3000);
    HMI_eraseString(&HANDLE_HMI, HMI_STRING_ADDRESS);
}