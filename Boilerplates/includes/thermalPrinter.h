#ifndef THERMALPRINTER_H
#define THERMALPRINTER_H

#include "main.h"

/**
 * This is based on the ESP/POS code, refer to general
 * documentation for extra data or refer to the printer
 * documentation
 */

/**
  * @brief  Initializes the thermal printer.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_init(UART_HandleTypeDef *huart);

/**
  * @brief  Prints text on the thermal printer.
  * @param  text: Pointer to a null-terminated string to be printed.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_print_text(const char *text, UART_HandleTypeDef *huart);

/**
  * @brief  Sends a line feed command to the thermal printer.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_line_feed(UART_HandleTypeDef *huart);

/**
  * @brief  Sets the bold attribute for text.
  * @param  on: Set to 1 to enable bold, 0 to disable.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_set_bold(uint8_t on, UART_HandleTypeDef *huart);

/**
  * @brief  Sets the text alignment for printing.
  * @param  alignment: Set to 0 for left, 1 for center, and 2 for right alignment.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_set_alignment(uint8_t alignment, UART_HandleTypeDef *huart);

/**
  * @brief  Sends a cut paper command to the thermal printer.
  * @param  huart: UART handle for the UART peripheral connected to the printer.
  * @retval None
  */
void printer_cut_paper(UART_HandleTypeDef *huart);

#endif /* THERMALPRINTER_H */
