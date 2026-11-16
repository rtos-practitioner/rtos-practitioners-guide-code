/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.12 Pitfall: Interrupt Priority Misconfiguration > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5, so
   configMAX_SYSCALL_INTERRUPT_PRIORITY = 5 << 4 = 0x50 */

void vSetupPeripherals(void)
{
    /* WRONG: Priority 2 is ABOVE configMAX_SYSCALL_INTERRUPT_PRIORITY.
       This ISR can preempt the FreeRTOS kernel! */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    /* CRASH: Calling FromISR from a priority above the RTOS ceiling */
    xSemaphoreGiveFromISR(xSemaphore, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}
