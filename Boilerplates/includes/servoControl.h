/*
 * servoControl.h
 *
 *  Created on: May 27, 2024
 *      Author: adith
 */
#include "stm32g4xx_hal.h"
#ifndef INC_SERVOCONTROL_H_
#define INC_SERVOCONTROL_H_

#define MSPERUL 4.4053

#include "stm32g4xx.h"
#include "main.h"
#include "FreeRTOS.h"
#include "stm32g4xx_hal_def.h"
#include "stm32g4xx_hal_cortex.h"

typedef enum {
    CUVETTE_STATE_IDLE,
    CUVETTE_STATE_RUNNING
} CuvetteState;

extern volatile CuvetteState cuvetteState ;
/**
  * @brief  Rotates the cuvette holder
  * @note   This is a state-less procedure and is not aware of the position of the cuvette
  * @note   The PWM is set at 100hz for the cuvette stepper
  * @param  htim20  This is the handle for timer 20 to which the step is connected to
  *
  * @retval none
  */
void cuvetteRotate(TIM_HandleTypeDef* htim20);

/**
  * @brief  Handles interrupt and stops cuvette
  * @retval none
  */
void cuvetteRotateHandler(void);

/**
  * @brief  Dispenses specified amount of liquid
  * @note   This is a blocking function. PWM is set at 1 revolution/second
  * @param  htimHandle: This is the handle for timer for which the stepper is connected to
  * @param  volume: Volume to be dispensed in ul
  * @param  pumpNum: Number of the pump
  *
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef dispenseStarter(TIM_HandleTypeDef* htimHandle, uint32_t volume, uint8_t pumpNum);


/**
  * @brief  Dispenses specified amounts of liquid using two pumps simultanously
  * @note   This is a blocking function. PWM is set at 1 revolution/second
  * @param  htim2: This is the handle for timer 2 to which the first pump is connected
  * @param  htim20: This is the handle for timer 20 to which the second pump is connected
  * @param  volume1: Volume to be dispensed by the first pump in ul
  * @param  volume2: Volume to be dispensed by the second pump in ul
  *
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef dispenseStarterSimul(TIM_HandleTypeDef* htim2,TIM_HandleTypeDef* htim20, uint32_t volume1, uint32_t volume2);

#endif /* INC_SERVOCONTROL_H_ */
