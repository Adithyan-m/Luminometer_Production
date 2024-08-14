/*
 * levelSense.h
 *
 *  Created on: May 23, 2024
 *      Author: Adithyan Mundayadu
 *      Project: Luminometer
 */

#ifndef INC_LEVELSENSE_H_
#define INC_LEVELSENSE_H_


#include "main_app.h"

// Device address
#define FDC1004_ADDR 0x50	// 7-bit address shifted left

// Register addresses
#define CONF_MEAS1 		0x08
#define CONF_MEAS2 		0x09
#define CONF_MEAS3 		0x0A
#define CONF_MEAS4 		0x0B
#define FDC_CONF   		0x0C
#define MEAS1_MSB  		0x00
#define MEAS1_LSB  		0x01
#define MEAS2_MSB		0x02
#define MEAS2_LSB		0x03
#define MEAS3_MSB		0x04
#define MEAS3_LSB		0x05
#define MEAS4_MSB		0x06
#define MEAS4_LSB		0x07


/**
  * @brief  Initializes FDC1004
  * @note   This is a general function and not a function to configure it based
  * 		on the needs of the luminometer, User needs to specify the way to configure
  * 		using the parameters
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains the configuration
  * 		information for the specified I2C.
  * @param 	measChnl Selects one of the 4 channels to use in the specified configuration (1-4)
  * @param 	measConfig Selects one of the 4 registers to save the specific configuration into (1-4)
  * @param  capdac This is to enable or disable cabdac
  * @param  capdacVal 5 bit value for capdac. If capdac is disabled pass 0
  * @retval HAL status
  */
HAL_StatusTypeDef initLevelSense_SingleEnded(I2C_HandleTypeDef *hi2c, uint8_t measChnl, uint8_t measConfig, bool capdec,uint8_t capdacVal );



/**
  * @brief  Function to read a single value from the IC
  * @note 	This is a blocking function and will not return function until value is read
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains the configuration
  * 		information for the specified I2C.
  * @param 	measConfig Selects one of the 4 registers where config is stored
  * @param 	measVal Pointer to a variable to store the value from the IC
  * @retval HAL status
  */
HAL_StatusTypeDef readValue_singleEnded(I2C_HandleTypeDef *hi2c, uint8_t measConfig, uint16_t* measVal);


#endif /* INC_LEVELSENSE_H_ */
