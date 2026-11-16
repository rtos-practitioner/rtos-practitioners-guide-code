/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.7 Pitfall: Polling Instead of Blocking > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
volatile BaseType_t xDataReady = pdFALSE;

/* ISR sets the flag */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    xDataReady = pdTRUE;
}

/* Task polls the flag in a tight loop */
void vDataTask(void *pvParam)
{
    for (;;) {
        while (xDataReady == pdFALSE) {
            /* BUG: spins at 168 MHz doing nothing useful.
               All lower-priority tasks are starved. */
        }
        xDataReady = pdFALSE;
        vProcessData();
    }
}
