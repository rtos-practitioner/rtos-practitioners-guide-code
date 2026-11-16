/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.2 Queue Fundamentals > Creating a Queue
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Queue holding up to 10 sensor readings, each 12 bytes */
QueueHandle_t xSensorQueue = xQueueCreate( 10, sizeof( SensorReading_t ) );
configASSERT( xSensorQueue != NULL );
