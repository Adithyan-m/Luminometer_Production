/*
 * thermalPrinter.c
 *
 *  Created on: Jun 11, 2024
 *      Author: adith
 */



#include "main.h"
#include "thermalPrinter.h"

// Function to initialize the printer
void printer_init(UART_HandleTypeDef * huart) {
    uint8_t init_cmd[] = {0x1B, 0x40};
    HAL_UART_Transmit(huart, init_cmd, sizeof(init_cmd), HAL_MAX_DELAY);
}

// Function to print text
void printer_print_text(const char *text, UART_HandleTypeDef * huart) {
    HAL_UART_Transmit(huart, (uint8_t *)text, strlen(text), HAL_MAX_DELAY);
}

// Function to line feed
void printer_line_feed(UART_HandleTypeDef * huart) {
    uint8_t lf_cmd[] = {0x0A};
    HAL_UART_Transmit(huart, lf_cmd, sizeof(lf_cmd), HAL_MAX_DELAY);
}

// Function to set text format (bold)
void printer_set_bold(uint8_t on, UART_HandleTypeDef * huart) {
    uint8_t bold_cmd[] = {0x1B, 0x45, on};
    HAL_UART_Transmit(huart, bold_cmd, sizeof(bold_cmd), HAL_MAX_DELAY);
}

// Function to set alignment (0: left, 1: center, 2: right)
void printer_set_alignment(uint8_t alignment, UART_HandleTypeDef * huart) {
    uint8_t align_cmd[] = {0x1B, 0x61, alignment};
    HAL_UART_Transmit(huart, align_cmd, sizeof(align_cmd), HAL_MAX_DELAY);
}

// Function to cut paper
void printer_cut_paper(UART_HandleTypeDef * huart) {
    uint8_t cut_cmd[] = {0x1D, 0x56, 0x00};
    HAL_UART_Transmit(huart, cut_cmd, sizeof(cut_cmd), HAL_MAX_DELAY);
}

