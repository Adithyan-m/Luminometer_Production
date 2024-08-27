/*
 * systemcheck.c
 *
 *  Created on: Aug 24, 2024
 *      Author: adith
 */

#include "systemcheck.h"
#include "states.h"


void autoCheck(void){

    // Switch page to "place cuvette and press okay" 
    uint8_t databuffer[10] = {0};
    while(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY != HAL_OK){
        __NOP();
    }

    if(databuffer[9] != AA){
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCEHCK_CUV_FAILED;
        return;  
    }

    // TODO: add correct timer handle
    cuvetteRotate(&htim1);

    // Background check
    uint32_t newBackground = PULSES_background(&htim2);
    WriteBackground(newBackground);


    // Press start priming pump
    uint8_t databuffer[10] = {0};
    while(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY != HAL_OK){
        __NOP();
    }

    if(databuffer[9] != AA){
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCEHCK_CUV_FAILED;
        return;  
    }

    // TODO : correct handle
    cuvetteRotate(&htim2);

    primePump(&htim2);
    cuvetteRotate(&htim2);

    uint8_t databuffer[10] = {0};
    while(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY != HAL_OK){
        __NOP();
    }
    // hello world i made a change here 
    if(databuffer[9] == AA){
        
        primePump(&htim2);
        cuvetteRotate(&htim2);
    }

    uint8_t databuffer[10] = {0};
    while(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY != HAL_OK){
        __NOP();
    }

    if(databuffer[9] == AA){
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCHECK_PRIME_FAILED;
        return;
    }

    // Press start Light Check
    uint8_t databuffer[10] = {0};
    while(HAL_UART_Receive(&huart2, dataBuffer, 10, HAL_MAX_DELAY != HAL_OK){
        __NOP();
    }

    if(databuffer[9] != AA){
        StateMachine.bEventOccurred = true;
        StateMachine.eEvent = EVENT_SYSCEHCK_CUV_FAILED;
        return;  
    }

    

}   

void backgroundCheck(void){

}

void pumpPriming(void){

}

void darkCheck(void){

}

void lightCheck(void){
    
}
