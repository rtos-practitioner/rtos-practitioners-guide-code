/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 2: Development Environment, Project Layout, and Board Bring-Up
 * 2.8 First Two Tasks > Heartbeat Task (Priority 2)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vHeartbeatTask(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(500);

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);  /* Green LED */
    }
}
