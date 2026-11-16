/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.6 Deadlock > 5.6.3 Code Example: Deadlock and Its Fix
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xMutexUART;   /* Protects UART peripheral */
SemaphoreHandle_t xMutexLog;    /* Protects log buffer */

void vTaskSensor(void *pv)
{
    for (;;)
    {
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* Lock UART first */
        xSemaphoreTake(xMutexLog,  portMAX_DELAY);  /* Then lock Log */
        /* Write sensor reading to log via UART */
        xSemaphoreGive(xMutexLog);
        xSemaphoreGive(xMutexUART);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vTaskDiagnostic(void *pv)
{
    for (;;)
    {
        xSemaphoreTake(xMutexLog,  portMAX_DELAY);  /* Lock Log first */
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* Then lock UART */
        /* Dump diagnostic log over UART */
        xSemaphoreGive(xMutexUART);
        xSemaphoreGive(xMutexLog);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
