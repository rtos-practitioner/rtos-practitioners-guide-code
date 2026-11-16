/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.7 Pitfall: Polling Instead of Blocking > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xDataReadySemaphore;

void vSystemInit(void)
{
    xDataReadySemaphore = xSemaphoreCreateBinary();
}

/* ISR signals the semaphore */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    xSemaphoreGiveFromISR(xDataReadySemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Task blocks efficiently until data is ready */
void vDataTask(void *pvParam)
{
    for (;;) {
        /* Task sleeps here, consuming zero CPU.
           Lower-priority tasks run. Idle task runs.
           Tickless idle can engage. */
        if (xSemaphoreTake(xDataReadySemaphore, portMAX_DELAY) == pdPASS) {
            vProcessData();
        }
    }
}
