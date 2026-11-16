/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.3 Sending and Receiving > Sending to a Queue
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xQueueSend( QueueHandle_t xQueue,
                       const void *pvItemToQueue,
                       TickType_t xTicksToWait );

BaseType_t xQueueSendToBack( QueueHandle_t xQueue,
                             const void *pvItemToQueue,
                             TickType_t xTicksToWait );

BaseType_t xQueueSendToFront( QueueHandle_t xQueue,
                              const void *pvItemToQueue,
                              TickType_t xTicksToWait );
