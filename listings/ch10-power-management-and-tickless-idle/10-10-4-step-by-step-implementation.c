/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void RTC_WKUP_IRQHandler(void)
{
    if (RTC->ISR & RTC_ISR_WUTF)
    {
        RTC->ISR &= ~RTC_ISR_WUTF;   /* Clear RTC flag  */
        EXTI->PR  = (1U << 22);        /* Clear EXTI flag */
    }
}
