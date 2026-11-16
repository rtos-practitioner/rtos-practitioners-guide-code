/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 2: Development Environment, Project Layout, and Board Bring-Up
 * 2.8 First Two Tasks > Monitor Task (Priority 4)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static uint32_t monitor_count = 0;

void vMonitorTask(void *pvParameters) {
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(100);

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
        monitor_count++;
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);  /* Blue LED */

        if (monitor_count % 50 == 0) {
            printf("[MON] count=%lu heap=%u tick=%lu\r\n",
                   monitor_count,
                   (unsigned)xPortGetFreeHeapSize(),
                   (unsigned long)xTaskGetTickCount());
        }
    }
}
