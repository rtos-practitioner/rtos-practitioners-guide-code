/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.6 Pitfall: ISR Calling Non-Thread-Safe APIs > Code Example
 * Verification: anti-pattern: fails exactly as the book describes
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xDataSemaphore;
QueueHandle_t xDataQueue;

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    uint32_t ulSensorValue = READ_SENSOR();

    /* WRONG: xQueueSend may block and corrupts kernel state in ISR */
    xQueueSend(xDataQueue, &ulSensorValue, portMAX_DELAY);

    /* WRONG: xSemaphoreTake must never be called from ISR */
    xSemaphoreTake(xDataSemaphore, portMAX_DELAY);
}
