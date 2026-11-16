/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.2 Queue Fundamentals > Creating a Queue
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static StaticQueue_t xQueueStorage;
static uint8_t ucQueueBuffer[ 10 * sizeof( SensorReading_t ) ];

QueueHandle_t xSensorQueue = xQueueCreateStatic(
    10,
    sizeof( SensorReading_t ),
    ucQueueBuffer,
    &xQueueStorage );
