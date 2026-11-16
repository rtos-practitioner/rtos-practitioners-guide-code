/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.8 Pitfall: Misusing vTaskDelay for Periodic Timing > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vSamplingTask(void *pvParam)
{
    for (;;) {
        vReadADCAndStoreResult();  /* Takes 1-5 ms depending on channel */

        /* WRONG: Delay is relative to NOW, not to the last wake.
           Period = execution_time + 100 ms, with jitter. */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
