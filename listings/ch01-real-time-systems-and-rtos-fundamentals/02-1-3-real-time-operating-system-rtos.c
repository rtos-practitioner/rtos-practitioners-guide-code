/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 1: Real-Time Systems and RTOS Fundamentals
 * 1.3 Bare Metal vs RTOS vs Linux > Real-Time Operating System (RTOS)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vControlTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
        read_sensor();
        compute_output();
        write_actuator();
    }
}

void vCommsTask(void *pvParameters) {
    for (;;) {
        Message_t msg;
        if (xQueueReceive(xCommsQueue, &msg, pdMS_TO_TICKS(100))) {
            process_message(&msg);
        }
    }
}
