/*
* flash.h
*
* Created on: Aug 10, 2024
* Author: adith
*/

#ifndef MAIN_APP_INCLUDES_FLASH_H_
#define MAIN_APP_INCLUDES_FLASH_H_

#include "main_app.h"

// Define the data types
typedef enum {
    DATA_TYPE_ERROR_CODES,
    DATA_TYPE_GENERAL_DATA,
    DATA_TYPE_PRODUCT_INFO,
    DATA_TYPE_DARK_COUNT,  // Add this new type
    DATA_TYPE_MAX
} DataType;

// Define the memory layout
#define FLASH_USER_START_ADDR 0x08060000
#define FLASH_USER_END_ADDR 0x0807FFFF

#define ERROR_CODES_START_ADDR FLASH_USER_START_ADDR
#define ERROR_CODES_SIZE 0x4000 // 16 KB for error codes

#define GENERAL_DATA_START_ADDR (ERROR_CODES_START_ADDR + ERROR_CODES_SIZE)
#define GENERAL_DATA_SIZE 0x17000 // 92 KB for general data

#define DARK_COUNT_START_ADDR (GENERAL_DATA_START_ADDR + GENERAL_DATA_SIZE)
#define DARK_COUNT_SIZE 0x1000 // 4 KB for dark count

#define PRODUCT_INFO_START_ADDR (DARK_COUNT_START_ADDR + DARK_COUNT_SIZE)
#define PRODUCT_INFO_SIZE 0x4000 // 16 KB for product info

#define RECORD_HEADER_SIZE 8 // 4 bytes for size, 4 bytes for status
#define RECORD_MAX_SIZE 1024 // Maximum size of a single record

typedef struct {
    uint32_t size;
    uint32_t status; // 0xFFFFFFFF = empty, 0x00000000 = valid, 0x00000001 = deleted
} RecordHeader;

HAL_StatusTypeDef PerformGarbageCollection(DataType type);
HAL_StatusTypeDef AppendData(DataType type, const uint8_t* data, uint32_t size);
HAL_StatusTypeDef ReadData(DataType type, uint32_t index, uint8_t* data, uint32_t* size);
HAL_StatusTypeDef DeleteData(DataType type, uint32_t index);
HAL_StatusTypeDef PerformGarbageCollection(DataType type);
uint32_t GetSectorForAddress(uint32_t address);
HAL_StatusTypeDef WriteDataToFlash(uint32_t address, const uint8_t* data, uint32_t size);

// functions for dark count
HAL_StatusTypeDef WriteDarkCount(uint32_t dark_count);
HAL_StatusTypeDef ReadLatestDarkCount(uint32_t* dark_count);

#endif /* MAIN_APP_INCLUDES_FLASH_H_ */
