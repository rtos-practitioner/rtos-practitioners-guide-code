/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.7 Design Patterns for Low Power > Event-Driven Architecture
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* BAD: polling prevents sleep */
void vBadTask(void *pv) {
    for (;;) {
        if (sensor_data_ready)       /* Spins forever */
            ProcessData();
    }
}

/* GOOD: event-driven, enables sleep */
void vGoodTask(void *pv) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  /* Blocks, CPU sleeps */
        ProcessData();
    }
}
