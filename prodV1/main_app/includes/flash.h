/*
 * flash.h
 *
 * Created on: Aug 10, 2024
 * Author: adith
 */

#ifndef MAIN_APP_INCLUDES_FLASH_H_
#define MAIN_APP_INCLUDES_FLASH_H_

#include "main_app.h"
#include <stdint.h>

// Define the data types
typedef enum {
    DATA_TYPE_ERROR_CODES,
    DATA_TYPE_GENERAL_DATA,
    DATA_TYPE_PRODUCT_INFO,
    DATA_TYPE_LIGHT_CHECK,
    DATA_TYPE_DARK_COUNT,
    DATA_TYPE_CURVED,
    DATA_TYPE_BACKGROUND,  // Add this new type
    DATA_TYPE_MAX
} DataType;

// Define the memory layout
#define FLASH_USER_START_ADDR 0x08060000
#define FLASH_USER_END_ADDR   0x0807FFFF

#define ERROR_CODES_START_ADDR FLASH_USER_START_ADDR
#define ERROR_CODES_SIZE       0x4000 // 16 KB for error codes

#define GENERAL_DATA_START_ADDR (ERROR_CODES_START_ADDR + ERROR_CODES_SIZE)
#define GENERAL_DATA_SIZE       0x10000 // 64 KB for general data

#define LIGHT_CHECK_START_ADDR (GENERAL_DATA_START_ADDR + GENERAL_DATA_SIZE)
#define LIGHT_CHECK_SIZE       0x2000 // 8 KB for light check

#define DARK_COUNT_START_ADDR (LIGHT_CHECK_START_ADDR + LIGHT_CHECK_SIZE)
#define DARK_COUNT_SIZE       0x2000 // 8 KB for dark count

#define CURVED_START_ADDR (DARK_COUNT_START_ADDR + DARK_COUNT_SIZE)
#define CURVED_SIZE       0x7000 // 28 KB for curved data

#define PRODUCT_INFO_START_ADDR (CURVED_START_ADDR + CURVED_SIZE)
#define PRODUCT_INFO_SIZE       0x4000 // 16 KB for product info

#define RECORD_HEADER_SIZE 16 // 4 bytes for size, 4 bytes for status, 8 bytes for timestamp
#define RECORD_MAX_SIZE 1024 // Maximum size of a single record

#define BACKGROUND_START_ADDR (CURVED_START_ADDR + CURVED_SIZE)
#define BACKGROUND_SIZE 0x1000 // 4 KB for background data

// Update PRODUCT_INFO_START_ADDR
#define PRODUCT_INFO_START_ADDR (BACKGROUND_START_ADDR + BACKGROUND_SIZE)

typedef uint64_t TimeStamp;

typedef struct {
    uint32_t size;
    uint32_t status; // 0xFFFFFFFF = empty, 0x00000000 = valid, 0x00000001 = deleted
    TimeStamp timestamp;
} RecordHeader;

typedef struct {
    uint16_t id;
    uint16_t type; // 0 for master, 1 for derived
    uint16_t data[10];
} CurvedData;

// Function declarations
HAL_StatusTypeDef PerformGarbageCollection(DataType type);
HAL_StatusTypeDef AppendData(DataType type, const uint8_t* data, uint32_t size);
HAL_StatusTypeDef ReadData(DataType type, uint32_t index, uint8_t* data, uint32_t* size);
HAL_StatusTypeDef DeleteData(DataType type, uint32_t index);
uint32_t GetSectorForAddress(uint32_t address);
HAL_StatusTypeDef WriteDataToFlash(uint32_t address, const uint8_t* data, uint32_t size);

HAL_StatusTypeDef WriteLightCheck(uint32_t light_check);
HAL_StatusTypeDef ReadLatestLightCheck(uint32_t* light_check, TimeStamp* timestamp);
HAL_StatusTypeDef WriteDarkCount(uint32_t dark_count);
HAL_StatusTypeDef ReadLatestDarkCount(uint32_t* dark_count, TimeStamp* timestamp);
HAL_StatusTypeDef WriteCurvedData(const CurvedData* curved_data);
HAL_StatusTypeDef ReadCurvedData(uint16_t id, uint16_t type, CurvedData* curved_data, TimeStamp* timestamp);
HAL_StatusTypeDef WriteBackground(uint32_t newBackground);
HAL_StatusTypeDef ReadLatestBackground(uint32_t* background, TimeStamp* timestamp);


TimeStamp GetCurrentTimestamp(void);

#endif /* MAIN_APP_INCLUDES_FLASH_H_ */