/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.4 ISR Design Patterns > Pattern 3: ISR with Critical Immediate Action
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (EXTI->PR & EXTI_PR_PR0)
    {
        EXTI->PR = EXTI_PR_PR0;                  /* Clear pending     */

        /* IMMEDIATE: disable motor driver output */
        GPIOB->BSRR = GPIO_BSRR_BR8;            /* PB8 low = disable */
        TIM1->BDTR &= ~TIM_BDTR_MOE;             /* Master output off */

        /* DEFERRED: notify fault handler task */
        xTaskNotifyFromISR(xFaultTaskHandle, FAULT_OVERCURRENT,
                           eSetBits, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
