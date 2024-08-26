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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Dark Threshold
#define DARK_THRESH 800

// PAGE NUMBERS
enum {
	PAGE_SPASH,
	PAGE_INIT,
	PAGE_MENU,
	PAGE_SERVICE_PASSWORD,
	PAGE_TEST_REPORTS,
	PAGE_QC_REPORTS,
	PAGE_SETTINGS,
	PAGE_SYSTEMS_CHECK,
	PAGE_MESURE,
	PAGE_CONTINUE,
	PAGE_SEVICES
};

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


HAL_StatusTypeDef HMI_changepage(UART_HandleTypeDef *uart, uint16_t pagenum);
HAL_StatusTypeDef HMI_writeString(UART_HandleTypeDef *uart, uint16_t vpAddr, const char* string);



// Main loop
void main_app(void);

#endif /* MAIN_APP_INCLUDES_MAIN_APP_H_ */
