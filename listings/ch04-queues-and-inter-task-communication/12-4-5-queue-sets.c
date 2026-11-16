/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.5 Queue Sets
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Create a set large enough for all member queues */
QueueSetHandle_t xSet = xQueueCreateSet(
    UART_QUEUE_LEN + ADC_QUEUE_LEN + NET_QUEUE_LEN );

/* Add queues to the set */
xQueueAddToSet( xUartQueue, xSet );
xQueueAddToSet( xAdcQueue,  xSet );
xQueueAddToSet( xNetQueue,  xSet );
