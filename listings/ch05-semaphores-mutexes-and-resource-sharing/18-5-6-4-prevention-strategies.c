/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.6 Deadlock > 5.6.4 Prevention Strategies
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
if (xSemaphoreTake(xMutexUART, pdMS_TO_TICKS(50)) != pdTRUE)
{
    /* Release already-held mutexes and retry */
    xSemaphoreGive(xMutexLog);
    /* Back off, log warning, or take corrective action */
}
