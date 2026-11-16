/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vControlTask(void *pvParam)
{
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10));
        xSemaphoreTake(xSharedBusSemaphore, portMAX_DELAY);
        vPerformBusTransaction();     /* 2 ms */
        xSemaphoreGive(xSharedBusSemaphore);
    }
}
/* xSharedBusSemaphore was created with xSemaphoreCreateBinary() */
/* Three tasks at priorities 1, 3, and 5 all use this semaphore */
