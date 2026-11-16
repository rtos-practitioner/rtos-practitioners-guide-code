/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.8 Design Patterns and Best Practices > Mailbox Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
QueueHandle_t xMailbox = xQueueCreate( 1, sizeof( SystemStatus_t ) );

/* Writer: always overwrites with the latest status */
xQueueOverwrite( xMailbox, &xCurrentStatus );

/* Reader: peeks to get the latest without removing it */
xQueuePeek( xMailbox, &xStatusCopy, portMAX_DELAY );
