/*
 * flash.c
 *
 * Created on: Aug 10, 2024
 * Author: adith
 */

#include "flash.h"
#include "stm32f4xx_hal.h"
#include <string.h>

static uint32_t last_write_address[DATA_TYPE_MAX] = {
    ERROR_CODES_START_ADDR,
    GENERAL_DATA_START_ADDR,
    PRODUCT_INFO_START_ADDR,
    LIGHT_CHECK_START_ADDR,
    DARK_COUNT_START_ADDR,
    CURVED_START_ADDR,
    BACKGROUND_START_ADDR
};

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
        case DATA_TYPE_LIGHT_CHECK:
            *start = LIGHT_CHECK_START_ADDR;
            *end = LIGHT_CHECK_START_ADDR + LIGHT_CHECK_SIZE;
            break;
        case DATA_TYPE_DARK_COUNT:
            *start = DARK_COUNT_START_ADDR;
            *end = DARK_COUNT_START_ADDR + DARK_COUNT_SIZE;
            break;
        case DATA_TYPE_CURVED:
            *start = CURVED_START_ADDR;
            *end = CURVED_START_ADDR + CURVED_SIZE;
            break;
        case DATA_TYPE_BACKGROUND:
            *start = BACKGROUND_START_ADDR;
            *end = BACKGROUND_START_ADDR + BACKGROUND_SIZE;
        break;
        default:
            *start = 0;
            *end = 0;
    }
}

static HAL_StatusTypeDef EraseFlashSector(uint32_t sector) {
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t SectorError = 0;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = sector;
    EraseInitStruct.NbSectors = 1;

    return HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
}

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

void ReadFromFlashAddress(uint32_t address, uint8_t* data, uint32_t size) {
    memcpy(data, (void*)address, size);
}

uint32_t GetSectorForAddress(uint32_t address) {
    if (address >= 0x08060000 && address < 0x0807FFFF) return FLASH_SECTOR_7;
    return 0xFFFFFFFF; // Invalid sector
}

HAL_StatusTypeDef WriteDataToFlash(uint32_t address, const uint8_t* data, uint32_t size) {
    uint32_t sector = GetSectorForAddress(address);
    if (sector == 0xFFFFFFFF) {
        return HAL_ERROR; // Invalid address
    }

    HAL_StatusTypeDef status;

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

static HAL_StatusTypeDef FindNextWriteAddress(DataType type) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t address = start;
    RecordHeader header;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0xFFFFFFFF) {
            last_write_address[type] = address;
            return HAL_OK;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef AppendData(DataType type, const uint8_t* data, uint32_t size) {
    if (size > RECORD_MAX_SIZE) {
        return HAL_ERROR;
    }

    if (FindNextWriteAddress(type) != HAL_OK) {
        if (PerformGarbageCollection(type) != HAL_OK) {
            return HAL_ERROR;
        }
        if (FindNextWriteAddress(type) != HAL_OK) {
            return HAL_ERROR;
        }
    }

    RecordHeader header = {
        .size = size,
        .status = 0x00000000,
        .timestamp = GetCurrentTimestamp()
    };

    HAL_StatusTypeDef status = WriteDataToFlash(last_write_address[type], (uint8_t*)&header, sizeof(RecordHeader));
    if (status != HAL_OK) {
        return status;
    }

    status = WriteDataToFlash(last_write_address[type] + sizeof(RecordHeader), data, size);
    if (status != HAL_OK) {
        return status;
    }

    last_write_address[type] += RECORD_HEADER_SIZE + size;
    last_write_address[type] = (last_write_address[type] + 3) & ~3;

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

        if (header.status == 0x00000000) {
            if (current_index == index) {
                ReadFromFlashAddress(address + sizeof(RecordHeader), data, header.size);
                *size = header.size;
                return HAL_OK;
            }
            current_index++;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef DeleteData(DataType type, uint32_t index) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t current_index = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {
            if (current_index == index) {
                uint32_t deleted_status = 0x00000001;
                return WriteDataToFlash(address + 4, (uint8_t*)&deleted_status, 4);
            }
            current_index++;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef PerformGarbageCollection(DataType type) {
    uint32_t start, end;
    GetAddressRange(type, &start, &end);

    uint32_t read_address = start;
    uint32_t write_address = start;
    RecordHeader header;

    while (read_address < end) {
        ReadFromFlashAddress(read_address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {
            if (read_address != write_address) {
                uint8_t buffer[RECORD_MAX_SIZE];
                ReadFromFlashAddress(read_address + sizeof(RecordHeader), buffer, header.size);

                WriteDataToFlash(write_address, (uint8_t*)&header, sizeof(RecordHeader));
                WriteDataToFlash(write_address + sizeof(RecordHeader), buffer, header.size);
            }
            write_address += RECORD_HEADER_SIZE + header.size;
            write_address = (write_address + 3) & ~3;
        }

        read_address += RECORD_HEADER_SIZE + header.size;
        read_address = (read_address + 3) & ~3;
    }

    uint32_t sector = GetSectorForAddress(write_address);
    while (sector <= GetSectorForAddress(end - 1)) {
        EraseFlashSector(sector);
        sector++;
    }

    last_write_address[type] = write_address;
    return HAL_OK;
}

TimeStamp GetCurrentTimestamp(void) {
    static TimeStamp placeholder_timestamp = 0;
    return placeholder_timestamp++;
}

HAL_StatusTypeDef WriteLightCheck(uint32_t light_check) {
    return AppendData(DATA_TYPE_LIGHT_CHECK, (uint8_t*)&light_check, sizeof(uint32_t));
}

HAL_StatusTypeDef ReadLatestLightCheck(uint32_t* light_check, TimeStamp* timestamp) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_LIGHT_CHECK, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;
    TimeStamp latest_timestamp = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000 && header.timestamp > latest_timestamp) {
            latest_valid_address = address;
            latest_timestamp = header.timestamp;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    if (latest_valid_address != 0) {
        ReadFromFlashAddress(latest_valid_address + sizeof(RecordHeader), (uint8_t*)light_check, sizeof(uint32_t));
        *timestamp = latest_timestamp;
        return HAL_OK;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef WriteDarkCount(uint32_t dark_count) {
    return AppendData(DATA_TYPE_DARK_COUNT, (uint8_t*)&dark_count, sizeof(uint32_t));
}

HAL_StatusTypeDef ReadLatestDarkCount(uint32_t* dark_count, TimeStamp* timestamp) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_DARK_COUNT, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;
    TimeStamp latest_timestamp = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000 && header.timestamp > latest_timestamp) {
            latest_valid_address = address;
            latest_timestamp = header.timestamp;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    if (latest_valid_address != 0) {
        ReadFromFlashAddress(latest_valid_address + sizeof(RecordHeader), (uint8_t*)dark_count, sizeof(uint32_t));
        *timestamp = latest_timestamp;
        return HAL_OK;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef WriteCurvedData(const CurvedData* curved_data) {
    return AppendData(DATA_TYPE_CURVED, (uint8_t*)curved_data, sizeof(CurvedData));
}

HAL_StatusTypeDef ReadCurvedData(uint16_t id, uint16_t type, CurvedData* curved_data, TimeStamp* timestamp) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_CURVED, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;
    TimeStamp latest_timestamp = 0;
    CurvedData temp_data;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000) {
            ReadFromFlashAddress(address + sizeof(RecordHeader), (uint8_t*)&temp_data, sizeof(CurvedData));

            if (temp_data.id == id && temp_data.type == type) {
                if (type == 0 || (type == 1 && header.timestamp > latest_timestamp)) {
                    latest_valid_address = address;
                    latest_timestamp = header.timestamp;
                }
            }
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    if (latest_valid_address != 0) {
        ReadFromFlashAddress(latest_valid_address + sizeof(RecordHeader), (uint8_t*)curved_data, sizeof(CurvedData));
        *timestamp = latest_timestamp;
        return HAL_OK;
    }

    return HAL_ERROR;
}

HAL_StatusTypeDef WriteBackground(uint32_t newBackground) {
    return AppendData(DATA_TYPE_BACKGROUND, (uint8_t*)&newBackground, sizeof(uint32_t));
}

HAL_StatusTypeDef ReadLatestBackground(uint32_t* background, TimeStamp* timestamp) {
    uint32_t start, end;
    GetAddressRange(DATA_TYPE_BACKGROUND, &start, &end);

    uint32_t address = start;
    RecordHeader header;
    uint32_t latest_valid_address = 0;
    TimeStamp latest_timestamp = 0;

    while (address < end) {
        ReadFromFlashAddress(address, (uint8_t*)&header, sizeof(RecordHeader));

        if (header.status == 0x00000000 && header.timestamp > latest_timestamp) {
            latest_valid_address = address;
            latest_timestamp = header.timestamp;
        }

        address += RECORD_HEADER_SIZE + header.size;
        address = (address + 3) & ~3;
    }

    if (latest_valid_address != 0) {
        ReadFromFlashAddress(latest_valid_address + sizeof(RecordHeader), (uint8_t*)background, sizeof(uint32_t));
        *timestamp = latest_timestamp;
        return HAL_OK;
    }

    return HAL_ERROR;
}