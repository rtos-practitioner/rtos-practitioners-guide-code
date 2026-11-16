/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.7 Temporal Determinism Strategies > 11.7.4 Drift-Free Periodic Execution
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* WRONG: vTaskDelay causes cumulative drift */
void vPeriodicTask(void *pvParameters) {
    for (;;) {
        do_work();          /* Variable execution time */
        vTaskDelay(pdMS_TO_TICKS(10));  /* 10 ms from NOW */
    }
}

/* CORRECT: vTaskDelayUntil maintains absolute period */
void vPeriodicTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(10);
    for (;;) {
        do_work();
        vTaskDelayUntil(&xLastWakeTime, xPeriod);  /* 10 ms from LAST WAKE */
    }
}
