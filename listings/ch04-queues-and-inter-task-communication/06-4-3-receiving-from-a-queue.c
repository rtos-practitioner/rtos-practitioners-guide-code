/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.3 Sending and Receiving > Receiving from a Queue
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xQueueReceive( QueueHandle_t xQueue,
                          void *pvBuffer,
                          TickType_t xTicksToWait );

BaseType_t xQueuePeek( QueueHandle_t xQueue,
                       void *pvBuffer,
                       TickType_t xTicksToWait );
