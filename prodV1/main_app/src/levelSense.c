/*
 * levelSense.c
 *
 *  Created on: May 23, 2024
 *      Author: Adithyan Mundayadu
 *      Project: Luminometer
 *
 */
#include "levelSense.h"
// Single ended configuration
HAL_StatusTypeDef initLevelSense_SingleEnded(I2C_HandleTypeDef *hi2c, uint8_t measChnl, uint8_t measConfig, bool capdac, uint8_t capdacVal ){

	// Checking for invalid parameter input
	if(measChnl < 1 || measChnl > 4 || measConfig < 1 || measConfig > 4){
		return HAL_ERROR;
	}

	if(capdac && (capdacVal > 0x1F)){
		return HAL_ERROR;
	}

	// Calculate address of config register
	uint8_t config_reg = CONF_MEAS1 + (measConfig-1);

	// Configuration for the initialization
	uint8_t config_Value = 0;

	// Set the CHA field (bits [15:13]) for the positive input pin
	config_Value |= (measChnl-1) << 13;

    // Set the CAPDAC field (bits [9:5]) if capdac is true
    if (capdac) {
        // Setting capdac value
        uint8_t capdac_value = capdacVal; // Example CAPDAC value
        config_Value |= (capdac_value << 5);
    }

    // Buffer for sending i2c (High byte first)
    uint8_t data_Buffer[3];
    data_Buffer[0] = config_reg & 0xFF;
    data_Buffer[1] = (config_Value>>8) & 0xFF;
    data_Buffer[2] = config_Value & 0xFF;

    // Sending on I2C
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, (FDC1004_ADDR<<1) , data_Buffer, 3, HAL_MAX_DELAY);
    return status;
}

// function to read a single value from the IC for specified config
HAL_StatusTypeDef readValue_singleEnded(I2C_HandleTypeDef *hi2c, uint8_t measConfig, uint16_t* measVal){

	// data buffer for sending
	uint8_t data_Buffer[3];

	// data buffer for data
    uint8_t msb_data[2];
    uint8_t lsb_data[2];

    // Wait for measurement completion (polling done flag)
    uint8_t reg = FDC_CONF;
    uint8_t status[2];

    data_Buffer[0] = FDC_CONF & 0xFF;
    data_Buffer[1] = 0x00;  // REPEAT disabled, RATE = 100S/s
    data_Buffer[2] = (0x01 << 7) >> (measConfig-1);  // MEAS1 enabled

    HAL_I2C_Master_Transmit(hi2c, (FDC1004_ADDR<<1) , data_Buffer, 3, HAL_MAX_DELAY);

    do {
            if (HAL_I2C_Master_Transmit(hi2c, (FDC1004_ADDR<<1), &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
                return HAL_ERROR;
            }
            if (HAL_I2C_Master_Receive(hi2c, (FDC1004_ADDR<<1), status, 2, HAL_MAX_DELAY) != HAL_OK) {
                return HAL_ERROR;
            }
        } while ((status[1] & (0x01 << (4 - measConfig))) == 0);  // Check DONE_n Flag

    reg = MEAS1_MSB + (2 * (measConfig - 1));


    // Getting MSB of the measured Data
    if (HAL_I2C_Master_Transmit(hi2c, FDC1004_ADDR, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
        return HAL_ERROR;
    }
    if (HAL_I2C_Master_Receive(hi2c, FDC1004_ADDR, msb_data, 2, HAL_MAX_DELAY) != HAL_OK) {
        return HAL_ERROR;
    }

    reg += 1;

    // Getting LSB of the measured Data
    if (HAL_I2C_Master_Transmit(hi2c, FDC1004_ADDR, &reg, 1, HAL_MAX_DELAY) != HAL_OK) {
            return HAL_ERROR;
        }
        if (HAL_I2C_Master_Receive(hi2c, FDC1004_ADDR, lsb_data, 2, HAL_MAX_DELAY) != HAL_OK) {
            return HAL_ERROR;
        }

    // Combine MSB and LSB to form the 24-bit result
    *measVal = ((uint32_t)msb_data[0] << 16) | ((uint32_t)msb_data[1] << 8) | lsb_data[0];

    return HAL_OK;

}

