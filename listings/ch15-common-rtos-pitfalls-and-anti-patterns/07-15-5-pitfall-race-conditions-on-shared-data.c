/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.5 Pitfall: Race Conditions on Shared Data > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
volatile uint32_t ulSharedCounter = 0;

/* Task A: increments counter */
void vProducerTask(void *pvParam)
{
    for (;;) {
        ulSharedCounter++;  /* NOT atomic: LDR, ADD, STR */
        /* Context switch can occur between LDR and STR.
           If Task B also increments, one increment is lost. */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* Task B: also increments counter */
void vConsumerTask(void *pvParam)
{
    for (;;) {
        ulSharedCounter++;  /* Same race condition */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
