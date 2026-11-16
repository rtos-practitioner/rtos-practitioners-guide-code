/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.10 Pitfall: Memory Leaks and Heap Fragmentation > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vPeriodicReportTask(void *pvParam)
{
    for (;;) {
        /* BUG: Creates a new one-shot timer every iteration.
           The timer is never deleted. Each creation leaks ~80 bytes. */
        TimerHandle_t xTimer = xTimerCreate(
            "Rpt", pdMS_TO_TICKS(500), pdFALSE, NULL, vReportCallback);
        xTimerStart(xTimer, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    /* After 1000 iterations: ~80 KB leaked. Heap exhausted. */
}
