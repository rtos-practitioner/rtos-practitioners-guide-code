/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.8 Measuring and Reducing Interrupt Latency > Measurement Techniques
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void EXTI1_IRQHandler(void)
{
    GPIOC->BSRR = GPIO_BSRR_BS0;    /* PC0 HIGH -- first instruction */

    /* ... ISR body ... */

    GPIOC->BSRR = GPIO_BSRR_BR0;    /* PC0 LOW -- last instruction */

    EXTI->PR = EXTI_PR_PR1;
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
