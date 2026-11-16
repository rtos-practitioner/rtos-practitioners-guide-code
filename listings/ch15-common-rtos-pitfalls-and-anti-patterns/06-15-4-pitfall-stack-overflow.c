/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.4 Pitfall: Stack Overflow > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Increased stack to 512 words (2048 bytes) with safety margin */
xTaskCreate(vLogTask, "Log", 512, NULL, 2, NULL);

void vLogTask(void *pvParam)
{
    /* Static buffer: allocated in .bss, not on the stack */
    static char cBuffer[512];

    for (;;) {
        snprintf(cBuffer, sizeof(cBuffer), "Sensor: %d, Temp: %d",
                 xReadSensor(), xReadTemperature());
        vSendToUART(cBuffer);

        /* Monitor stack usage in debug builds */
        #if (configCHECK_FOR_STACK_OVERFLOW >= 1)
        {
            UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
            configASSERT(uxHighWaterMark > 50);  /* At least 50 words free */
        }
        #endif

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
