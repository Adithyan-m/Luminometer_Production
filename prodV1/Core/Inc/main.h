/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "main_app.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STEP1_Pin GPIO_PIN_5
#define STEP1_GPIO_Port GPIOE
#define NTC_Pin GPIO_PIN_1
#define NTC_GPIO_Port GPIOC
#define HV_FB_Pin GPIO_PIN_2
#define HV_FB_GPIO_Port GPIOC
#define TX2_Pin GPIO_PIN_0
#define TX2_GPIO_Port GPIOA
#define RX2_Pin GPIO_PIN_1
#define RX2_GPIO_Port GPIOA
#define REF1_Pin GPIO_PIN_4
#define REF1_GPIO_Port GPIOA
#define REF2_Pin GPIO_PIN_5
#define REF2_GPIO_Port GPIOA
#define ESP_RST_Pin GPIO_PIN_6
#define ESP_RST_GPIO_Port GPIOA
#define OPTO2_Pin GPIO_PIN_7
#define OPTO2_GPIO_Port GPIOA
#define MS2_3_Pin GPIO_PIN_5
#define MS2_3_GPIO_Port GPIOC
#define MS1_3_Pin GPIO_PIN_0
#define MS1_3_GPIO_Port GPIOB
#define ENABLE3_Pin GPIO_PIN_1
#define ENABLE3_GPIO_Port GPIOB
#define DIR3_Pin GPIO_PIN_2
#define DIR3_GPIO_Port GPIOB
#define STEP3_Pin GPIO_PIN_9
#define STEP3_GPIO_Port GPIOE
#define OPTO1_Pin GPIO_PIN_12
#define OPTO1_GPIO_Port GPIOE
#define DIR2_Pin GPIO_PIN_13
#define DIR2_GPIO_Port GPIOE
#define MS2_2_Pin GPIO_PIN_14
#define MS2_2_GPIO_Port GPIOE
#define MS1_2_Pin GPIO_PIN_15
#define MS1_2_GPIO_Port GPIOE
#define ENABLE2_Pin GPIO_PIN_10
#define ENABLE2_GPIO_Port GPIOB
#define STEP2_Pin GPIO_PIN_12
#define STEP2_GPIO_Port GPIOD
#define RFID_RST_Pin GPIO_PIN_6
#define RFID_RST_GPIO_Port GPIOC
#define IRQ_Pin GPIO_PIN_8
#define IRQ_GPIO_Port GPIOC
#define SDA1_Pin GPIO_PIN_9
#define SDA1_GPIO_Port GPIOC
#define SCL1_Pin GPIO_PIN_8
#define SCL1_GPIO_Port GPIOA
#define TX3_Pin GPIO_PIN_9
#define TX3_GPIO_Port GPIOA
#define RX3_Pin GPIO_PIN_10
#define RX3_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWDCK_Pin GPIO_PIN_14
#define SWDCK_GPIO_Port GPIOA
#define SS_Pin GPIO_PIN_15
#define SS_GPIO_Port GPIOA
#define SCK_Pin GPIO_PIN_10
#define SCK_GPIO_Port GPIOC
#define MISO_Pin GPIO_PIN_11
#define MISO_GPIO_Port GPIOC
#define MOSI_Pin GPIO_PIN_12
#define MOSI_GPIO_Port GPIOC
#define TUBE_SEN_Pin GPIO_PIN_0
#define TUBE_SEN_GPIO_Port GPIOD
#define LIMIT_SW_1_Pin GPIO_PIN_1
#define LIMIT_SW_1_GPIO_Port GPIOD
#define TX_Pin GPIO_PIN_5
#define TX_GPIO_Port GPIOD
#define RX_Pin GPIO_PIN_6
#define RX_GPIO_Port GPIOD
#define ENABLE1_Pin GPIO_PIN_3
#define ENABLE1_GPIO_Port GPIOB
#define MS1_1_Pin GPIO_PIN_4
#define MS1_1_GPIO_Port GPIOB
#define MS2_1_Pin GPIO_PIN_5
#define MS2_1_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define PULSES_Pin GPIO_PIN_8
#define PULSES_GPIO_Port GPIOB
#define DIR1_Pin GPIO_PIN_9
#define DIR1_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOE
#define HV_DISABLE_Pin GPIO_PIN_1
#define HV_DISABLE_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
