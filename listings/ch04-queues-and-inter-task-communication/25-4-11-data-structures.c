/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.11 Lab 4: Multi-Channel Data Pipeline > Data Structures
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include <string.h>

#define QUEUE_DEPTH         32
#define NUM_SENSOR_TASKS    2
#define SENSOR_TEMP         0
#define SENSOR_PRESSURE     1
#define SENSOR_VIBRATION    2

typedef struct {
    uint8_t    ucSensorID;
    float      fValue;
    TickType_t xTimestamp;
} SensorCommand_t;

static QueueHandle_t   xCommandQueue;
static TaskHandle_t    xAggregatorHandle;
static volatile uint32_t ulIsrEnqueueFail = 0;
static volatile uint32_t ulTaskEnqueueFail = 0;
