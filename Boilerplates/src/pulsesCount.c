/*
 * pulsesCount.c
 *
 *  Created on: May 29, 2024
 *      Author: adith
 */
#include "pulseCount.h"

// Global variables for saving values from measurement
volatile uint32_t lastCapture = 0;
volatile float frequency = 0;
float frequencyBuffer[BUFFER_SIZE] = {0};
volatile uint32_t bufferIndex = 0;
volatile uint8_t measuring = 0;
volatile bool buffOverflow = false;
TIM_HandleTypeDef* tim5 = NULL;

void startMeasure(TIM_HandleTypeDef* htim5, bool useThresh, uint16_t threshVal) {
    tim5 = htim5;
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_TIM_Base_Start(htim5);

    // call function to start motor pump

    // Reset buffer and variables
    bufferIndex = 0;
    measuring = 1;

    // Wait for 3 seconds
    HAL_Delay(3000);

    if (useThresh) {
        while (frequency > threshVal) {
            // Delay for 100ms
            HAL_Delay(100);
        }

        // Stop the timer
        HAL_TIM_Base_Stop(htim5);

        // Disable interrupt
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    } else {
        // Stop the timer
        HAL_TIM_Base_Stop(htim5);

        // Disable interrupt
        HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    }
}
