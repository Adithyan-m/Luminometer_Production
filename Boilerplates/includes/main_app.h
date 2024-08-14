#ifndef _MAIN_APP_H
#define _MAIN_APP_H

// User includes
#include "main.h"
#include <servoControl.h>
#include "levelSense.h"
#include "rc522.h"
#include "string.h"
#include "pulseCount.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "thermalPrinter.h"

// Queue definitions
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(uint16_t)

#define HMICHECK 0xFF

// Main functoin
void main_app(void);

#endif
