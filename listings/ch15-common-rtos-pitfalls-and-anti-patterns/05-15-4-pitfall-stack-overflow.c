/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.4 Pitfall: Stack Overflow > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Task created with 256-word (1024-byte) stack */
xTaskCreate(vLogTask, "Log", 256, NULL, 2, NULL);

void vLogTask(void *pvParam)
{
    for (;;) {
        char cBuffer[512];  /* 512 bytes — half the stack, in one frame */
        /* After accounting for context save, local variables in called
           functions, and return addresses, this overflows */
        snprintf(cBuffer, sizeof(cBuffer), "Sensor: %d, Temp: %d",
                 xReadSensor(), xReadTemperature());
        vSendToUART(cBuffer);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
