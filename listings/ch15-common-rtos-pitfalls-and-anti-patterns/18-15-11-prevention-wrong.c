/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.11 Pitfall: Starving the Idle Task > Prevention
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* WRONG: CPU-bound task at high priority, never blocks */
void vBadCpuBoundTask(void *pvParam)
{
    for (;;) {
        vDoHeavyComputation();  /* Never yields — idle task starved */
    }
}

/* CORRECT: Task blocks periodically, allowing idle task to run */
void vGoodCpuBoundTask(void *pvParam)
{
    TickType_t xLastWake = xTaskGetTickCount();
    for (;;) {
        vDoHeavyComputation();
        /* Yield for at least 1 tick every iteration */
        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(1));
    }
}
