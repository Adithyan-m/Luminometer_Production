/*
 * pulses.h
 *
 *  Created on: Aug 11, 2024
 *      Author: adith
 */

#ifndef MAIN_APP_INCLUDES_PULSES_H_
#define MAIN_APP_INCLUDES_PULSES_H_

#include "main_app.h"

uint32_t PULSES_background(TIM_HandleTypeDef *htim);
uint32_t PULSES_darkcount(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);



#endif /* MAIN_APP_INCLUDES_PULSES_H_ */
