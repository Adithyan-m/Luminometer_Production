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

HAL_StatusTypeDef initLevelSensors(I2C_HandleTypeDef *hi2c) {
    HAL_StatusTypeDef status;

    // Setup measurement 1 for channel 1
    status = initLevelSense_SingleEnded(hi2c, 1, 1, false, 0);
    if (status != HAL_OK) return status;

    // Setup measurement 2 for channel 2
    status = initLevelSense_SingleEnded(hi2c, 2, 2, false, 0);
    if (status != HAL_OK) return status;

    // Setup measurement 3 for channel 3
    status = initLevelSense_SingleEnded(hi2c, 3, 3, false, 0);
    if (status != HAL_OK) return status;

    return HAL_OK;
}

HAL_StatusTypeDef readLevelSensors(I2C_HandleTypeDef *hi2c, float *channel1, float *channel2, float *channel3) {
    HAL_StatusTypeDef status;

    // Trigger measurements
    uint8_t fdc_conf[3] = {FDC_CONF, 0x04, 0x07}; // 100S/s, MEAS_1, MEAS_2, and MEAS_3 enabled
    status = HAL_I2C_Master_Transmit(hi2c, (FDC1004_ADDR<<1), fdc_conf, 3, HAL_MAX_DELAY);
    if (status != HAL_OK) return status;

    // Wait for measurements to complete
    uint8_t status_reg[2];
    do {
        uint8_t reg = FDC_CONF;
        status = HAL_I2C_Master_Transmit(hi2c, (FDC1004_ADDR<<1), &reg, 1, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;
        
        status = HAL_I2C_Master_Receive(hi2c, (FDC1004_ADDR<<1), status_reg, 2, HAL_MAX_DELAY);
        if (status != HAL_OK) return status;
    } while ((status_reg[1] & 0x07) != 0x07); // Wait for DONE_1, DONE_2, and DONE_3

    // Read values
    uint16_t raw_value;
    
    status = readValue_singleEnded(hi2c, 1, &raw_value);
    if (status != HAL_OK) return status;
    *channel1 = ((float)raw_value / 524288.0f) - 16.0f;

    status = readValue_singleEnded(hi2c, 2, &raw_value);
    if (status != HAL_OK) return status;
    *channel2 = ((float)raw_value / 524288.0f) - 16.0f;

    status = readValue_singleEnded(hi2c, 3, &raw_value);
    if (status != HAL_OK) return status;
    *channel3 = ((float)raw_value / 524288.0f) - 16.0f;

    return HAL_OK;
}

uint8_t calculate_level(I2C_HandleTypeDef *hi2c){
    
    float channel1, channel2, channel3;
    uint8_t level = 0;
    // Get the three readings
    readLevelSensors(hi2c,&channel1,&channel2,&channel3);

    // CH1 - LEV, CH2 - RE, CH3 - RL
    // (lev - lev0)/(rl - re)
    
    level = (channel1 - CLEV0)/(channel3 - channel2);

    return level;

}

