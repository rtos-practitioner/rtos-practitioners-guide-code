/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vAlertTask(void *pvParam)
{
    for (;;) {
        TimerHandle_t xT = xTimerCreate("Alert",
            pdMS_TO_TICKS(200), pdFALSE, NULL, vAlertCallback);
        xTimerStart(xT, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
