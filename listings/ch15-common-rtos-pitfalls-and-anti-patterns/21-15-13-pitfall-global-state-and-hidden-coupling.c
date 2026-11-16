/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.13 Pitfall: Global State and Hidden Coupling > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Global shared state — no synchronization, hidden dependencies */
volatile float gTemperature;
volatile float gPressure;
volatile uint8_t gDataValid;

void vSensorTask(void *pvParam)
{
    for (;;) {
        gTemperature = fReadTemperatureSensor();
        gPressure = fReadPressureSensor();
        gDataValid = 1;  /* Race: consumer might read between writes */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vDisplayTask(void *pvParam)
{
    for (;;) {
        if (gDataValid) {
            /* Race: might read new temperature with old pressure */
            vUpdateDisplay(gTemperature, gPressure);
            gDataValid = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
