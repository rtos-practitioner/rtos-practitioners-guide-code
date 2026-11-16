/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
volatile uint32_t ulErrorCount = 0;

void vCommTask(void *pvParam)     /* Priority 3 */
{
    for (;;) {
        if (bCheckForError()) ulErrorCount++;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vLogTask(void *pvParam)      /* Priority 2 */
{
    for (;;) {
        uint32_t ulSnapshot = ulErrorCount;
        ulErrorCount = 0;
        vLogErrorCount(ulSnapshot);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
