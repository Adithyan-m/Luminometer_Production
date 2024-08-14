/*
 * flash.c
 *
 *  Created on: Aug 10, 2024
 *      Author: adith
 */

#include "flash.h"

#include "stm32f4xx_hal.h"
#include <string.h>

static uint32_t last_write_address[DATA_TYPE_MAX] = {
    ERROR_CODES_START_ADDR,
    GENERAL_DATA_START_ADDR,
    PRODUCT_INFO_START_ADDR,
    DARK_COUNT_START_ADDR
};
// Helper function to get the start and end addresses for a given data type
static void GetAddressRange(DataType type, uint32_t* start, uint32_t* end) {
    switch (type) {

    	case DATA_TYPE_ERROR_CODES:
            *start = ERROR_CODES_START_ADDR;
            *end = ERROR_CODES_START_ADDR + ERROR_CODES_SIZE;
            break;

    	case DATA_TYPE_GENERAL_DATA:
            *start = GENERAL_DATA_START_ADDR;
            *end = GENERAL_DATA_START_ADDR + GENERAL_DATA_SIZE;
            break;

    	case DATA_TYPE_PRODUCT_INFO:
            *start = PRODUCT_INFO_START_ADDR;
            *end = PRODUCT_INFO_START_ADDR + PRODUCT_INFO_SIZE;
            break;

    	case DATA_TYPE_DARK_COUNT:
            *start = DARK_COUNT_START_ADDR;
            *end = DARK_COUNT_START_ADDR + DARK_COUNT_SIZE;
            break;

    	default:
            *start = 0;
            *end = 0;
    }
}


// Function to erase a sector
static HAL_StatusTypeDef EraseFlashSector(uint32_t sector) {
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = sector;
    EraseInitStruct.NbSectors = 1;

    return HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
}

// Function to write data to a specific address
HAL_StatusTypeDef WriteToFlashAddress(uint32_t address, const uint8_t* data, uint32_t size) {
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < size; i += 4) {
        uint32_t data_word = *(uint32_t*)&data[i];
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, data_word);
        if (status != HAL_OK) {
            break;
        }
    }

    HAL_FLASH_Lock();
    return status;
}

// Function to read data from a specific address
void ReadFromFlashAddress(uint32_t address, uint8_t* data, uint32_t size) {
    memcpy(data, (void*)address, size);
}

// Helper function to get the sector number for a given address
uint32_t GetSectorForAddress(uint32_t address) {

    if (address >= 0x08060000 && address < 0x0807FFFF) return FLASH_SECTOR_7;
    return 0xFFFFFFFF; // Invalid sector
}

// Function to write data, erasing the sector if necessary
HAL_StatusTypeDef WriteDataToFlash(uint32_t address, const uint8_t* data, uint32_t size) {
    uint32_t sector = GetSectorForAddress(address);
    if (sector == 0xFFFFFFFF) {
        return HAL_ERROR; // Invalid address
    }

    HAL_StatusTypeDef status;

    // Check if the sector needs to be erased
    uint32_t* flash_ptr = (uint32_t*)address;
    bool needs_erase = false;
    for (uint32_t i = 0; i < (size + 3) / 4; i++) {
        if (flash_ptr[i] != 0xFFFFFFFF) {
            needs_erase = true;
            break;
        }
    }

    if (needs_erase) {
        status = EraseFlashSector(sector);
        if (status != HAL_OK) {
            return status;
        }
    }

    return WriteToFlashAddress(address, data, size);
}

// gives back latest dark count
HAL_StatusTypeDef ReadLatestDarkCount(uint32_t* dark_count) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_DARK_COUNT, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {  // Valid record
            latest_valid_address = address;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;  // Align to 4-byte boundary
    }

    if (latest_valid_address != 0) {
        ReadFromFlashAddress(latest_valid_address + sizeof(RecordHeader), (uint8_t*)dark_count, sizeof(uint32_t));
        return HAL_OK;
    }

    return HAL_ERROR;  // No valid dark count found
}

// Function to find the next available write address for a given data type
static HAL_StatusTypeDef FindNextWriteAddress(DataType type) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t address = start;
    RecordHeader header;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0xFFFFFFFF) {
            // Found an empty spot
            last_write_address[type] = address;
            return HAL_OK;
        }

        // Move to the next record
        address += RECORD_HEADER_SIZE + header.size;

        // Align to 4-byte boundary
        address = (address + 3) & ~3;
    }

    // If we're here, we've run out of space
    return HAL_ERROR;
}

HAL_StatusTypeDef AppendData(DataType type, const uint8_t* data, uint32_t size) {
    if (size > RECORD_MAX_SIZE) {
        return HAL_ERROR;
    }

    if (FindNextWriteAddress(type) != HAL_OK) {
        // No space left, need to perform garbage collection
        if (PerformGarbageCollection(type) != HAL_OK) {
            return HAL_ERROR;
        }
        if (FindNextWriteAddress(type) != HAL_OK) {
            return HAL_ERROR;
        }
    }

    RecordHeader header = {size, 0x00000000};  // Valid status

    HAL_StatusTypeDef status = WriteDataToFlash(last_write_address[type], (uint8_t*)&header, sizeof(RecordHeader));
    if (status != HAL_OK) {
        return status;
    }

    status = WriteDataToFlash(last_write_address[type] + sizeof(RecordHeader), data, size);
    if (status != HAL_OK) {
        return status;
    }

    last_write_address[type] += RECORD_HEADER_SIZE + size;
    last_write_address[type] = (last_write_address[type] + 3) & ~3;  // Align to 4-byte boundary

    return HAL_OK;
}

HAL_StatusTypeDef ReadData(DataType type, uint32_t index, uint8_t* data, uint32_t* size) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t current_index = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {  // Valid record
            if (current_index == index) {
                ReadFromFlashAddress(address + sizeof(RecordHeader), data, header.size);
                *size = header.size;
                return HAL_OK;
            }
            current_index++;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;  // Align to 4-byte boundary
    }

    return HAL_ERROR;  // Record not found
}

HAL_StatusTypeDef DeleteData(DataType type, uint32_t index) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t current_index = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {  // Valid record
            if (current_index == index) {
                // Mark as deleted
                uint32_t deleted_status = 0x00000001;
                return WriteDataToFlash(address + 4, (uint8_t*)&deleted_status, 4);
            }
            current_index++;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;  // Align to 4-byte boundary
    }

    return HAL_ERROR;  // Record not found
}

HAL_StatusTypeDef WriteDarkCount(uint32_t dark_count) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_DARK_COUNT, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;

    // Find the latest valid dark count
    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {  // Valid record
            latest_valid_address = address;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;  // Align to 4-byte boundary
    }

    // Invalidate the previous dark count if it exists
    if (latest_valid_address != 0) {
        uint32_t invalidate_status = 0x00000001;
        HAL_StatusTypeDef status = WriteDataToFlash(latest_valid_address + 4, (uint8_t*)&invalidate_status, 4);
        if (status != HAL_OK) {
            return status;
        }
    }

    // Write the new dark count
    RecordHeader new_header = {sizeof(uint32_t), 0x00000000};  // Valid status
    HAL_StatusTypeDef status = AppendData(DATA_TYPE_DARK_COUNT, (uint8_t*)&dark_count, sizeof(uint32_t));
    if (status != HAL_OK) {
        return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef PerformGarbageCollection(DataType type) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t read_address = start;
    uint32_t write_address = start;
    RecordHeader header;

    while (read_address < end) {
        ReadFromFlashAddress(read_address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {  // Valid record
            if (read_address != write_address) {
                // Move the record
                uint8_t buffer[RECORD_MAX_SIZE];
                ReadFromFlashAddress(read_address + sizeof(RecordHeader), buffer, header.size);

                WriteDataToFlash(write_address, (uint8_t*)&header, sizeof(RecordHeader));
                WriteDataToFlash(write_address + sizeof(RecordHeader), buffer, header.size);
            }
            write_address += RECORD_HEADER_SIZE + header.size;
            write_address = (write_address + 3) & ~3;  // Align to 4-byte boundary
        }

        read_address += RECORD_HEADER_SIZE + header.size;
        read_address = (read_address + 3) & ~3;  // Align to 4-byte boundary
    }

    // Erase remaining space
    uint32_t sector = GetSectorForAddress(write_address);
    while (sector <= GetSectorForAddress(end - 1)) {
        EraseFlashSector(sector);
        sector++;
    }

    last_write_address[type] = write_address;
    return HAL_OK;
}
