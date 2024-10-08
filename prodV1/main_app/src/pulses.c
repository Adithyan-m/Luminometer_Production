/*
 * pulses.c
 *
 *  Created on: Aug 11, 2024
 *      Author: adith
 */
#include "pulses.h"

volatile uint32_t pulse_count = 0;

uint32_t PULSES_darkcount(TIM_HandleTypeDef *htim) {
  // Reset pulse count
  pulse_count = 0;

  // Start timer
  HAL_TIM_IC_Start(htim, TIM_CHANNEL_1);

  // Wait for 3 seconds
  HAL_Delay(3000);

  // Stop timer
  HAL_TIM_IC_Stop(htim, TIM_CHANNEL_1);

  // Calculate average pulses per second
  uint32_t average_pulses = pulse_count / 3;

  return average_pulses;
}

uint32_t PULSES_background(TIM_HandleTypeDef *htim){
  
  // Reset pulse count
  pulse_count = 0;

  // Start timer
  HAL_TIM_IC_Start(htim, TIM_CHANNEL_1);

  // Wait for 3 seconds
  HAL_Delay(3000);

  // Stop timer
  HAL_TIM_IC_Stop(htim, TIM_CHANNEL_1);

  // Calculate average pulses per second
  uint32_t average_pulses = pulse_count / 3;

  return average_pulses;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
  // Increment pulse count on each rising edge
  pulse_count++;
}
