/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.3 Sending and Receiving > Sending to a Queue
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
if( xQueueSend( xSensorQueue, &xReading, pdMS_TO_TICKS( 50 ) ) != pdPASS )
{
    /* Queue full for 50 ms --- log overflow, increment error counter */
    ulQueueOverflowCount++;
}
