/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
void vMonitorTask(void *pvParam)
{
    for (;;) {
        char cReport[2048];
        snprintf(cReport, sizeof(cReport),
                 "System Status: uptime=%lu ticks, free heap=%u bytes",
                 (unsigned long)xTaskGetTickCount(),
                 (unsigned)xPortGetFreeHeapSize());
        vSendReport(cReport);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
/* Task created with: xTaskCreate(vMonitorTask, "Mon", 256, ...) */
