/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.5 Pitfall: Race Conditions on Shared Data > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
uint32_t ulSharedCounter = 0;
SemaphoreHandle_t xCounterMutex;

void vSystemInit(void)
{
    xCounterMutex = xSemaphoreCreateMutex();
}

/* Option A: Mutex protection (preferred for longer operations) */
void vProducerTask(void *pvParam)
{
    for (;;) {
        xSemaphoreTake(xCounterMutex, portMAX_DELAY);
        ulSharedCounter++;
        xSemaphoreGive(xCounterMutex);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* Option B: Critical section (suitable for single-word operations) */
void vConsumerTask(void *pvParam)
{
    for (;;) {
        taskENTER_CRITICAL();
        ulSharedCounter++;
        taskEXIT_CRITICAL();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
