/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.8 Design Patterns and Best Practices > Timeout Strategy
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
if( xQueueSend( xQueue, &xData, pdMS_TO_TICKS( 100 ) ) != pdPASS )
{
    vReportError( ERR_QUEUE_TIMEOUT );
    /* Take corrective action: drop data, restart consumer, etc. */
}
