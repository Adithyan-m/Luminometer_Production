/*
 * servoControl.c
 *
 *  Created on: May 27, 2024
 *      Author: adith
 */

#include "servoControl.h"

volatile CuvetteState cuvetteState = CUVETTE_STATE_IDLE;
static TIM_HandleTypeDef* cuvetteHtim = NULL;

void cuvetteRotate(TIM_HandleTypeDef* htim20) {
    cuvetteHtim = htim20;

        // Start the PWM
        HAL_TIM_PWM_Start(htim20, TIM_CHANNEL_2);
        HAL_GPIO_WritePin(MS1_1_GPIO_Port, MS1_1_Pin,GPIO_PIN_RESET );
        HAL_GPIO_WritePin(MS2_1_GPIO_Port, MS2_1_Pin,GPIO_PIN_RESET );
        HAL_GPIO_WritePin(DIR_1_GPIO_Port, DIR_1_Pin,GPIO_PIN_SET );
        static int status = 0;

        while(1){
        	if(HAL_GPIO_ReadPin(LM_SW_GPIO_Port, LM_SW_Pin == GPIO_PIN_SET)){
        		status = 1;
        	}

        	if (HAL_GPIO_ReadPin(LM_SW_GPIO_Port, LM_SW_Pin == GPIO_PIN_RESET) && status){
        		HAL_TIM_PWM_Stop(htim20, TIM_CHANNEL_2);
        	}
        }
    }
}

void cuvetteRotateHandler(void) {
    if (cuvetteState == CUVETTE_STATE_RUNNING) {
        // Transition to stopped state
        cuvetteState = CUVETTE_STATE_IDLE;

        // Stop the PWM
        HAL_TIM_PWM_Stop(cuvetteHtim, TIM_CHANNEL_2);
        HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

        // Optionally handle other cleanup or state transitions
    }
}

HAL_StatusTypeDef dispenseStarter(TIM_HandleTypeDef* htimHandle, uint32_t volume, uint8_t pumpNum) {
    uint32_t delay = volume * MSPERUL;
    uint8_t channel = 0;

	// wait until cuvette is stopped
	while(cuvetteState == CUVETTE_STATE_RUNNING){
		vTaskDelay(100);
	}

    switch (pumpNum) {
        case 1:
            channel = TIM_CHANNEL_4;
            HAL_GPIO_WritePin(MS1_2_GPIO_Port, MS1_2_Pin,GPIO_PIN_RESET );
            HAL_GPIO_WritePin(MS2_2_GPIO_Port, MS2_2_Pin,GPIO_PIN_RESET );
            HAL_GPIO_WritePin(DIR_2_GPIO_Port, DIR_2_Pin,GPIO_PIN_SET );
            break;
        case 2:
            channel = TIM_CHANNEL_4;
            break;
            HAL_GPIO_WritePin(MS1_3_GPIO_Port, MS1_3_Pin,GPIO_PIN_RESET );
            HAL_GPIO_WritePin(MS2_3_GPIO_Port, MS2_3_Pin,GPIO_PIN_RESET );
            HAL_GPIO_WritePin(DIR_3_GPIO_Port, DIR_3_Pin,GPIO_PIN_SET );
        default:
            return HAL_ERROR;
    }

    // Start the PWM for dispensing
    HAL_TIM_PWM_Start(htimHandle, channel);



    vTaskDelay(delay); // Simple delay for the volume dispensation
    HAL_TIM_PWM_Stop(htimHandle, channel);

    return HAL_OK;
}


HAL_StatusTypeDef dispenseStarterSimul(TIM_HandleTypeDef* htim2,TIM_HandleTypeDef* htim20, uint32_t volume1, uint32_t volume2) {

	// wait until cuvette is stopped
	while(cuvetteState == CUVETTE_STATE_RUNNING){
		vTaskDelay(100);
	}

    uint32_t time1 = volume1 * MSPERUL;
    uint32_t time2 = volume2 * MSPERUL;
    uint32_t timeToRrun = (time1 > time2) ? time2 : time1;
    uint32_t diff = (time1 > time2) ? (time1 - time2) : (time2 - time1);

    // Start the PWM for dispensing
    HAL_TIM_PWM_Start(htim2, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(htim20, TIM_CHANNEL_1);


    HAL_GPIO_WritePin(MS1_2_GPIO_Port, MS1_2_Pin,GPIO_PIN_RESET );
    HAL_GPIO_WritePin(MS2_2_GPIO_Port, MS2_2_Pin,GPIO_PIN_RESET );
    HAL_GPIO_WritePin(DIR_2_GPIO_Port, DIR_2_Pin,GPIO_PIN_SET );

    HAL_GPIO_WritePin(MS1_3_GPIO_Port, MS1_3_Pin,GPIO_PIN_RESET );
    HAL_GPIO_WritePin(MS2_3_GPIO_Port, MS2_3_Pin,GPIO_PIN_RESET );
    HAL_GPIO_WritePin(DIR_2_GPIO_Port, DIR_2_Pin,GPIO_PIN_SET );

    vTaskDelay(timeToRrun); // Simple delay for the volume dispensation

    // To stop one pump if it has less volume
    if(time1>time2){
    	HAL_TIM_PWM_Stop(htim20, TIM_CHANNEL_1);
    }else{
    	HAL_TIM_PWM_Stop(htim2, TIM_CHANNEL_4);
    }

    // Continuing to dispense the starter
    vTaskDelay(diff);

    // To stop the remaining pump
    if(time1>time2){
    	HAL_TIM_PWM_Stop(htim2, TIM_CHANNEL_4);
    }else{
    	HAL_TIM_PWM_Stop(htim20, TIM_CHANNEL_1);
    }


    return HAL_OK;
}
