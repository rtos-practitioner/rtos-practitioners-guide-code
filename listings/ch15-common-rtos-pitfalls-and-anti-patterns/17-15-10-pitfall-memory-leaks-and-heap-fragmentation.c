/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.10 Pitfall: Memory Leaks and Heap Fragmentation > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static TimerHandle_t xReportTimer = NULL;

void vSystemInit(void)
{
    /* Create once during init */
    xReportTimer = xTimerCreate(
        "Rpt", pdMS_TO_TICKS(500), pdFALSE, NULL, vReportCallback);
    configASSERT(xReportTimer != NULL);
}

void vPeriodicReportTask(void *pvParam)
{
    for (;;) {
        /* Reuse the same timer — no allocation, no leak */
        xTimerStart(xReportTimer, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
