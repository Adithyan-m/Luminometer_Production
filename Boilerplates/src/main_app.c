#include "main_app.h"

// STM32 Peripheral handles referenced from main.c
extern I2C_HandleTypeDef hi2c4;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim20;
extern UART_HandleTypeDef huart4;

// Task Handles
//static TaskHandle_t hmiTaskHandle = NULL;
//static TaskHandle_t rfidTaskHandle = NULL;
//static QueueHandle_t uartQueue;

// Function Prototypes
//void hmiRecieveInputTask(void *);
//void rfidTask(void *);

// Global Variable declarations
uint8_t rxBuffer[8];
uint8_t rfidStatus;
uint8_t rfidStr[MAX_LEN]; // Max_LEN = 16
uint8_t rfidSNum[5];

void main_app(void) {

//    int8_t status;
//
//    uartQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
//    configASSERT(uartQueue != NULL);
//
//    status = xTaskCreate(hmiRecieveInputTask, "HMI_IN", 1024, NULL, tskIDLE_PRIORITY, &hmiTaskHandle);
//    configASSERT(status == pdPASS);
//
//    status = xTaskCreate(rfidTask, "rfid", 1024, NULL, tskIDLE_PRIORITY, &rfidTaskHandle);
//    configASSERT(status == pdPASS);
//
//    // Start UART receive in interrupt mode
//    HAL_UART_Receive_IT(&huart4, rxBuffer, 8);
//
//    vTaskStartScheduler();

    while (1) {

    	cuvetteRotate(&htim20);

    }
}

//// Task to handles button clicks from HMI
//void hmiRecieveInputTask(void *pvParameters) {
//
//    uint8_t receivedData;
//
//    // Main loop
//    for (;;) {
//
//        // Wait indefinitely for data from the queue
//        if (xQueueReceive(uartQueue, &receivedData, portMAX_DELAY) == pdPASS) {
//
//            // Process the received data
//            do {
//
//                // To check if it is a button click or other data
//                if ((receivedData >> 8) == HMICHECK) {
//
//                    //Parsing button click
//                    switch (receivedData & 0x00FF) {
//
//                        case 0x01: // measure task
//                            cuvetteRotate(&htim20);
//                            dispenseStarterSimul(&htim2, &htim20, 10, 20);
//                            break;
//                        // Handle other cases if needed
//
//                        case 0x02: // scan rfid task
//
//                    }
//                }
//                // Check if there is more data in the queue
//            } while (xQueueReceive(uartQueue, &receivedData, 0) == pdPASS);
//        }
//    }
//}
//
//
//// Task to handle the rfid card reader
//void rfidTask(void *pvParameters) {
//
//    // Initialize RFID Reader
//    // TODO: Cross check pin definitions in .h file
//    MFRC522_Init();
//
//    for (;;) {
//
//        // Getting data from scanner
//        rfidStatus = MFRC522_Request(PICC_REQIDL, rfidStr);
//        rfidStatus = MFRC522_Anticoll(rfidStr);
//        memcpy(rfidSNum, rfidStr, 5);
//        vTaskDelay(50);
//    }
//}
//
//// UART Interrupt Callback
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == huart4.Instance) {
//
//    	// Combine the received bytes into a 64-bit word
//        uint64_t receivedData = 0;
//        for (int i = 0; i < 8; i++) {
//            receivedData |= ((uint64_t)rxBuffer[i] << (56 - 8 * i));
//        }
//
//        // Send the received word to the queue
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xQueueSendFromISR(uartQueue, &receivedData, &xHigherPriorityTaskWoken);
//
//        // Restart UART receive in interrupt mode
//        HAL_UART_Receive_IT(&huart4, rxBuffer, 8);
//
//        // Request a context switch if sending to the queue woke up a higher priority task
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//    }
//}
