/*
 * main_app.h
 *
 * Created on: Aug 6, 2024
 * Author: adith
 */

#ifndef MAIN_APP_INCLUDES_MAIN_APP_H_
#define MAIN_APP_INCLUDES_MAIN_APP_H_

#include "main.h"
#include "states.h"
#include "stdbool.h"
#include "flash.h"
#include "stepperDriver.h"
#include "pulses.h"
#include "stdint.h"
#include "levelSense.h"
#include "stdint.h"

// Dark Threshold
#define DARK_THRESH 800

// Peripheral Handles
extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim9;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;



// Main loop
void main_app(void);

#endif /* MAIN_APP_INCLUDES_MAIN_APP_H_ */
