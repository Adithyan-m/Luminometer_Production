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

    // Background check
    uint32_t = PULSES_background(&htim2);

    // Rotate cuvete
    // do check
    // ask if to store    
    
    
    // Prime pump
    // Run three cycles
    // Spin cuvette
    // Ask if cuvette has water
    // if no spin again with a "tried once flag"
    // spin again and ask
    // if no again, pump faliure error


    // light check 
    // ask for skip or add reagenet
    // spin 
    // display results

    

}   

void backgroundCheck(void){

}

void pumpPriming(void){

}

void darkCheck(void){

}

void lightCheck(void){
    
}
