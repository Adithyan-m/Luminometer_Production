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

typedef enum {
    DATA_TYPE_ERROR_CODES,
    DATA_TYPE_GENERAL_DATA,
    DATA_TYPE_PRODUCT_INFO,
    DATA_TYPE_LIGHT_CHECK,
    DATA_TYPE_DARK_COUNT,
    DATA_TYPE_CURVED,
    DATA_TYPE_BACKGROUND,
    DATA_TYPE_MAX
} DataType;

// Define the memory layout

#define FLASH_USER_START_ADDR 0x08060000
#define FLASH_USER_END_ADDR 0x0807FFFF

#define ERROR_CODES_START_ADDR FLASH_USER_START_ADDR
#define ERROR_CODES_SIZE 0x4000 // 16 KB for error codes

#define GENERAL_DATA_START_ADDR (ERROR_CODES_START_ADDR + ERROR_CODES_SIZE)
#define GENERAL_DATA_SIZE 0x10000 // 64 KB for general data

#define PRODUCT_INFO_START_ADDR (GENERAL_DATA_START_ADDR + GENERAL_DATA_SIZE)
#define PRODUCT_INFO_SIZE 0x8000 // Assuming 32 KB for product info (not specified earlier)

#define LIGHT_CHECK_START_ADDR (PRODUCT_INFO_START_ADDR + PRODUCT_INFO_SIZE)
#define LIGHT_CHECK_SIZE 0x2000 // 8 KB for light check

#define DARK_COUNT_START_ADDR (LIGHT_CHECK_START_ADDR + LIGHT_CHECK_SIZE)
#define DARK_COUNT_SIZE 0x2000 // 8 KB for dark count

#define CURVED_START_ADDR (DARK_COUNT_START_ADDR + DARK_COUNT_SIZE)
#define CURVED_SIZE 0x7000 // 28 KB for curved data

#define BACKGROUND_START_ADDR (CURVED_START_ADDR + CURVED_SIZE)
#define BACKGROUND_SIZE 0x1000 // 4 KB for background data

#define RECORD_HEADER_SIZE 16 // 4 bytes for size, 4 bytes for status, 8 bytes for timestamp
#define RECORD_MAX_SIZE 1024 // Maximum size of a single record

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
/**
 * @brief  Initializes the flash memory for the specified data type
 * @note   This function finds the next available write address for the specified data type.
 *         It checks for empty records and returns the address of the first empty record.
 * @param  type  The type of data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef FindNextWriteAddress(DataType type);

/**
 * @brief  Writes data to the flash memory
 * @note   This function writes the data to the next available write address for the specified data type.
 *         It appends a new record to the end of the data type's memory region.
 * @param  type  The type of data to be written
 * @param  data  The data to be written
 * @param  size  The size of the data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef AppendData(DataType type, const uint8_t* data, uint32_t size);

/**
 * @brief  Reads data from the flash memory
 * @note   This function reads the data from the flash memory for the specified data type and index.
 *         It returns the data and its size in the provided buffers.
 * @param  type  The type of data to be read
 * @param  index The index of the data to be read
 * @param  data  The buffer to store the read data
 * @param  size  The size of the data to be read
 * @retval HAL status
 */
HAL_StatusTypeDef ReadData(DataType type, uint32_t index, uint8_t* data, uint32_t* size);

/**
 * @brief  Deletes data from the flash memory
 * @note   This function marks the data as deleted for the specified data type and index.
 *         It sets the status of the record to deleted (0x00000001).
 * @param  type  The type of data to be deleted
 * @param  index The index of the data to be deleted
 * @retval HAL status
 */
HAL_StatusTypeDef DeleteData(DataType type, uint32_t index);

/**
 * @brief  Performs garbage collection on the flash memory
 * @note   This function removes deleted data and compacts the flash memory for the specified data type.
 *         It rewrites valid records to contiguous memory locations.
 * @param  type  The type of data to perform garbage collection on
 * @retval HAL status
 */
HAL_StatusTypeDef PerformGarbageCollection(DataType type);

/**
 * @brief  Writes light check data to the flash memory
 * @note   This function writes the light check data to the flash memory.
 *         It appends a new record to the end of the light check data memory region.
 * @param  light_check The light check data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef WriteLightCheck(uint32_t light_check);

/**
 * @brief  Reads the latest light check data from the flash memory
 * @note   This function reads the latest light check data from the flash memory.
 *         It returns the latest light check data and its timestamp.
 * @param  light_check The buffer to store the latest light check data
 * @param  timestamp The timestamp of the latest light check data
 * @retval HAL status
 */
HAL_StatusTypeDef ReadLatestLightCheck(uint32_t* light_check, TimeStamp* timestamp);

/**
 * @brief  Writes dark count data to the flash memory
 * @note   This function writes the dark count data to the flash memory.
 *         It appends a new record to the end of the dark count data memory region.
 * @param  dark_count The dark count data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef WriteDarkCount(uint32_t dark_count);

/**
 * @brief  Reads the latest dark count data from the flash memory
 * @note   This function reads the latest dark count data from the flash memory.
 *         It returns the latest dark count data and its timestamp.
 * @param  dark_count The buffer to store the latest dark count data
 * @param  timestamp The timestamp of the latest dark count data
 * @retval HAL status
 */
/**
 * @brief  Reads the latest dark count data from the flash memory
 * @note   This function reads the latest dark count data from the flash memory.
 *         It returns the latest dark count data and its timestamp.
 *         The latest data is determined by the highest timestamp.
 * @param  dark_count The buffer to store the latest dark count data
 * @param  timestamp The timestamp of the latest dark count data
 * @retval HAL status
 */
HAL_StatusTypeDef ReadLatestDarkCount(uint32_t* dark_count, TimeStamp* timestamp);

/**
 * @brief  Writes curved data to the flash memory
 * @note   This function writes the curved data to the flash memory.
 *         It appends a new record to the end of the curved data memory region.
 *         The record includes the curved data and its timestamp.
 * @param  curved_data The curved data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef WriteCurvedData(const CurvedData* curved_data);

/**
 * @brief  Reads curved data from the flash memory
 * @note   This function reads the curved data from the flash memory for the specified ID and type.
 *         It returns the curved data and its timestamp.
 *         The function searches for the first matching record with the specified ID and type.
 * @param  id           The ID of the curved data to be read
 * @param  type         The type of the curved data to be read
 * @param  curved_data  The buffer to store the read curved data
 * @param  timestamp    The timestamp of the read curved data
 * @retval HAL status
 */
HAL_StatusTypeDef ReadCurvedData(uint16_t id, uint16_t type, CurvedData* curved_data, TimeStamp* timestamp);

/**
 * @brief  Writes background data to the flash memory
 * @note   This function writes the background data to the flash memory.
 *         It overwrites the existing background data with the new value.
 * @param  newBackground The new background data to be written
 * @retval HAL status
 */
HAL_StatusTypeDef WriteBackground(uint32_t newBackground);

/**
 * @brief  Reads the latest background data from the flash memory
 * @note   This function reads the latest background data from the flash memory.
 *         It returns the latest background data and its timestamp.
 *         The latest data is determined by the highest timestamp.
 * @param  background The buffer to store the latest background data
 * @param  timestamp  The timestamp of the latest background data
 * @retval HAL status
 */
HAL_StatusTypeDef ReadLatestBackground(uint32_t* background, TimeStamp* timestamp);

/**
 * @brief  Gets the current timestamp
 * @note   This function returns the current timestamp.
 *         The timestamp is a 64-bit value representing the number of milliseconds since a fixed point in time (e.g., January 1, 1970).
 * @retval Current timestamp
 */
TimeStamp GetCurrentTimestamp(void);

#endif /* MAIN_APP_INCLUDES_FLASH_H_ */
