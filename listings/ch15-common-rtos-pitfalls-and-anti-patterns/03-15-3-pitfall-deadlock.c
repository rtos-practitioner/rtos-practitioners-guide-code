/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.3 Pitfall: Deadlock > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xMutexUART;   /* Conceptual order: 1 */
SemaphoreHandle_t xMutexSPI;    /* Conceptual order: 2 */

/* Task A: acquires UART then SPI */
void vTaskA(void *pvParam)
{
    for (;;) {
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* Lock 1 */
        xSemaphoreTake(xMutexSPI, portMAX_DELAY);   /* Lock 2 */
        /* BUG: If Task B holds SPI and waits for UART → deadlock */
        vTransferDataUARTtoSPI();
        xSemaphoreGive(xMutexSPI);
        xSemaphoreGive(xMutexUART);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* Task B: acquires SPI then UART — OPPOSITE ORDER */
void vTaskB(void *pvParam)
{
    for (;;) {
        xSemaphoreTake(xMutexSPI, portMAX_DELAY);   /* Lock 2 */
        xSemaphoreTake(xMutexUART, portMAX_DELAY);  /* Lock 1 */
        /* DEADLOCK possible: holds SPI, waits for UART */
        vTransferDataSPItoUART();
        xSemaphoreGive(xMutexUART);
        xSemaphoreGive(xMutexSPI);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
