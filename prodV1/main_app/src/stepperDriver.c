/*
 * stepperDriver.c
 *
 *  Created on: Jul 14, 2024
 *      Author: adith
 */

#include "main_app.h"
#include "stepperDriver.h"


HAL_StatusTypeDef cuvetteRotate(TIM_HandleTypeDef* tim_handle, DAC_HandleTypeDef* dac_handle) {

	// Start the PWM
    HAL_TIM_PWM_Start(tim_handle, TIM_CHANNEL_1);

    // Set Micro-step
    HAL_GPIO_WritePin(MS1_1_GPIO_Port, MS1_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MS2_1_GPIO_Port, MS2_1_Pin, GPIO_PIN_RESET);

    if( HAL_DAC_Start(dac_handle, DAC_CHANNEL_1) != HAL_OK){
    	Error_Handler();
    }

    if (HAL_DAC_SetValue(dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2000) != HAL_OK){
    	Error_Handler();
    }

    // Set Direction
    HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_SET);

    GPIO_PinState lastState = GPIO_PIN_RESET;
    GPIO_PinState currentState;
    uint32_t startTime = HAL_GetTick();
    uint32_t timeout = 300; // 30 seconds timeout

    while (1) {
        currentState = HAL_GPIO_ReadPin(LIMIT_SW_1_GPIO_Port, LIMIT_SW_1_Pin);
    	HAL_DAC_SetValue(dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x100);

        // Detect falling edge
        if (lastState == GPIO_PIN_SET && currentState == GPIO_PIN_RESET) {
            HAL_TIM_PWM_Stop(tim_handle, TIM_CHANNEL_1);
            return HAL_OK;
        }

        lastState = currentState;

        // Check for timeout
        if (HAL_GetTick() - startTime > timeout) {
            HAL_TIM_PWM_Stop(tim_handle, TIM_CHANNEL_1);
            return HAL_TIMEOUT;
        }

        HAL_Delay(10);
    }
}


//
HAL_StatusTypeDef dispenseStarter(TIM_HandleTypeDef* htimHandle, DAC_HandleTypeDef* dac_handle, uint32_t volume, uint8_t pumpNum) {
   uint32_t delay = volume * MSPERUL;
   uint8_t channel = 0;
    
    if( HAL_DAC_Start(dac_handle, DAC_CHANNEL_2) != HAL_OK){
    	Error_Handler();
    }

    if (HAL_DAC_SetValue(dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 2000) != HAL_OK){
    	Error_Handler();
    }

   switch (pumpNum) {
       case 1:
           HAL_GPIO_WritePin(MS1_2_GPIO_Port, MS1_2_Pin,GPIO_PIN_RESET );
           HAL_GPIO_WritePin(MS2_2_GPIO_Port, MS2_2_Pin,GPIO_PIN_RESET );
           HAL_GPIO_WritePin(DIR2_GPIO_Port, DIR2_Pin,GPIO_PIN_SET );
           break;
       case 2:
           HAL_GPIO_WritePin(MS1_3_GPIO_Port, MS1_3_Pin,GPIO_PIN_RESET );
           HAL_GPIO_WritePin(MS2_3_GPIO_Port, MS2_3_Pin,GPIO_PIN_RESET );
           HAL_GPIO_WritePin(DIR3_GPIO_Port, DIR3_Pin,GPIO_PIN_SET );
           break;
       default:
           return HAL_ERROR;
   }

   // Start the PWM for dispensing
   HAL_TIM_PWM_Start(htimHandle, TIM_CHANNEL_1);
   
   HAL_Delay(delay); // Simple delay for the volume dispensation

   HAL_TIM_PWM_Stop(htimHandle, TIM_CHANNEL_1);

   return HAL_OK;
}

HAL_StatusTypeDef primePump(TIM_HandleTypeDef *timPump1, TIM_HandleTypeDef *timPump2, DAC_HandleTypeDef* dac_handle){

  dispenseStarter(timPump1,dac_handle, 20, 1);
  dispenseStarter(timPump2,dac_handle, 20, 2);
  dispenseStarter(timPump1,dac_handle, 20, 1);
  dispenseStarter(timPump2,dac_handle, 20, 2);
  dispenseStarter(timPump1,dac_handle, 20, 1);
  dispenseStarter(timPump2,dac_handle, 20, 2);

  return HAL_OK;
}


HAL_StatusTypeDef dispenseStarterSimul(TIM_HandleTypeDef* timPump1, DAC_HandleTypeDef* dac_handle,TIM_HandleTypeDef* timPump2, uint32_t volume1, uint32_t volume2) {

	// wait until cuvette is stopped

        if( HAL_DAC_Start(dac_handle, DAC_CHANNEL_2) != HAL_OK){
    	Error_Handler();
    }

    if (HAL_DAC_SetValue(dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 2000) != HAL_OK){
    	Error_Handler();
    }


   uint32_t time1 = volume1 * MSPERUL;
   uint32_t time2 = volume2 * MSPERUL;
   uint32_t timeToRrun = (time1 > time2) ? time2 : time1;
   uint32_t diff = (time1 > time2) ? (time1 - time2) : (time2 - time1);

   // Start the PWM for dispensing
   HAL_TIM_PWM_Start(timPump1, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(timPump2, TIM_CHANNEL_1);


   HAL_GPIO_WritePin(MS1_2_GPIO_Port, MS1_2_Pin,GPIO_PIN_RESET );
   HAL_GPIO_WritePin(MS2_2_GPIO_Port, MS2_2_Pin,GPIO_PIN_RESET );
   HAL_GPIO_WritePin(DIR2_GPIO_Port, DIR2_Pin,GPIO_PIN_SET );

   HAL_GPIO_WritePin(MS1_3_GPIO_Port, MS1_3_Pin,GPIO_PIN_RESET );
   HAL_GPIO_WritePin(MS2_3_GPIO_Port, MS2_3_Pin,GPIO_PIN_RESET );
   HAL_GPIO_WritePin(DIR2_GPIO_Port, DIR2_Pin,GPIO_PIN_SET );

   HAL_Delay(timeToRrun); // Simple delay for the volume dispensation

   // To stop one pump if it has less volume
   if(time1>time2){
   	HAL_TIM_PWM_Stop(timPump2, TIM_CHANNEL_1);
   }else{
   	HAL_TIM_PWM_Stop(timPump1, TIM_CHANNEL_1);
   }

   // Continuing to dispsense the starter
   HAL_Delay(diff);

   // To stop the remaining pump
   if(time1>time2){
   	HAL_TIM_PWM_Stop(timPump1, TIM_CHANNEL_4);
   }else{
   	HAL_TIM_PWM_Stop(timPump2, TIM_CHANNEL_1);
   }


   return HAL_OK;
}
