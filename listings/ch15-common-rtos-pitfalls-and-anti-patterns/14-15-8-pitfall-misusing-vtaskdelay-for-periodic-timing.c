/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.8 Pitfall: Misusing vTaskDelay for Periodic Timing > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vSamplingTask(void *pvParam)
{
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = pdMS_TO_TICKS(100);

    /* Initialize with the current tick count */
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vReadADCAndStoreResult();  /* Takes 1-5 ms — does not matter */

        /* CORRECT: Delays until xLastWakeTime + xPeriod.
           xLastWakeTime is automatically advanced by xPeriod.
           Period stays 100 ms (no drift) as long as execution plus
           preemption fits within the period. On an overrun the call
           returns immediately (xTaskDelayUntil() returns pdFALSE) and
           the deadline has been missed. */
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}
