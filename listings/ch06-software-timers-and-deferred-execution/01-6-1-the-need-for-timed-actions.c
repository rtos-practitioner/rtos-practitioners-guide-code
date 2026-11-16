/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.1 The Need for Timed Actions
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vWatchdogFeedTask(void *pvParameters)
{
    for (;;) {
        HAL_IWDG_Refresh(&hiwdg);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
