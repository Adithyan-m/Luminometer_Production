/*
 * pulseCount.h
 *
 *  Created on: May 29, 2024
 *      Author: adith
 */

#include "stdbool.h"
#include "main.h"

// Size of buffer for storing frequency values
#define BUFFER_SIZE 3000

#ifndef INC_PULSECOUNT_H_
#define INC_PULSECOUNT_H_

// Global variables for saving values from measurement
extern volatile uint32_t lastCapture;
extern volatile float frequency;
extern float frequencyBuffer[BUFFER_SIZE];
extern volatile uint32_t bufferIndex;
extern volatile uint8_t measuring;
extern volatile bool buffOverflow;
extern TIM_HandleTypeDef* tim5;

/**
  * @brief  Function starts the pulse counting sequence and saves the value to the
  * 		global variable called frequency buffer.
  * @param  useThresh: If this value is true then it will stop measurement when the
  * 		frequency falls below a particular threshold.
  * @param  threshVal: This value specifies the threshold for the measurement.
  * @note   When using this function for measuring the background noise, useThresh should
  * 		be kept at 0.
  * @retval None
  */
void startMeasure(TIM_HandleTypeDef* htim5, bool useThresh, uint16_t threshVal);

#endif /* INC_PULSECOUNT_H_ */
