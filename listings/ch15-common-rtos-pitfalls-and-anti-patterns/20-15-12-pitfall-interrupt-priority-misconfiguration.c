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
    /* CORRECT: Priority 5 is at the RTOS ceiling — safe to call
       FromISR APIs. Priorities 5-15 are RTOS-safe. */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    /* Priority 1 ISR — fast, but must NOT call any FreeRTOS API */
    HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void EXTI0_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    /* SAFE: Priority 5, within RTOS-managed range */
    xSemaphoreGiveFromISR(xSemaphore, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    /* Priority 1: ultra-low-latency timer — NO FreeRTOS calls allowed */
    GPIOD->ODR ^= GPIO_PIN_12;  /* Direct register access only */
}
