/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.2 Pitfall: Unbounded Priority Inversion > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* CORRECT: Mutex enables priority inheritance */
SemaphoreHandle_t xI2CMutex;

void vSystemInit(void)
{
    xI2CMutex = xSemaphoreCreateMutex();  /* Created in "given" state */
}

/* Low-priority task (priority 1) */
void vSensorTask(void *pvParam)
{
    for (;;) {
        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
        /* While this task holds the mutex, if ControlTask blocks on it,
           the kernel elevates SensorTask to priority 3 temporarily */
        vReadSensorOverI2C();
        xSemaphoreGive(xI2CMutex);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* Medium-priority task (priority 2) */
void vProcessingTask(void *pvParam)
{
    for (;;) {
        vPerformHeavyComputation();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* High-priority task (priority 3) */
void vControlTask(void *pvParam)
{
    for (;;) {
        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
        /* Priority inheritance ensures SensorTask runs at priority 3
           while holding the mutex, preventing ProcessingTask from
           causing unbounded inversion */
        vSendCommandOverI2C();
        xSemaphoreGive(xI2CMutex);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
