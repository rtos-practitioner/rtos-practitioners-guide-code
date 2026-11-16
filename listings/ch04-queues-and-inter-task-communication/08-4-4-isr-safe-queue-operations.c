/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.4 ISR-Safe Queue Operations
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xQueueSendFromISR( QueueHandle_t xQueue,
                              const void *pvItemToQueue,
                              BaseType_t *pxHigherPriorityTaskWoken );

BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue,
                                 void *pvBuffer,
                                 BaseType_t *pxHigherPriorityTaskWoken );
