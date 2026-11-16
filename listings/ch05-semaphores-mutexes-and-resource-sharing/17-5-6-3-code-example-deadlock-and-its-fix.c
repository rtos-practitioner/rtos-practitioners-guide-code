/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.6 Deadlock > 5.6.3 Code Example: Deadlock and Its Fix
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* RULE: Always acquire xMutexLog BEFORE xMutexUART */

void vTaskSensor(void *pv)
{
    for (;;)
    {
        xSemaphoreTake(xMutexLog,  portMAX_DELAY);  /* Log first */
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* UART second */
        /* Write sensor reading to log via UART */
        xSemaphoreGive(xMutexUART);
        xSemaphoreGive(xMutexLog);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vTaskDiagnostic(void *pv)
{
    for (;;)
    {
        xSemaphoreTake(xMutexLog,  portMAX_DELAY);  /* Log first */
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* UART second */
        /* Dump diagnostic log over UART */
        xSemaphoreGive(xMutexUART);
        xSemaphoreGive(xMutexLog);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
