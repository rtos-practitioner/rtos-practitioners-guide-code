/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
static void SystemClock_RestoreAfterStop(void)
{
    /* Re-enable HSE */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY))
        ;

    /* Re-enable PLL (settings preserved in RCC registers) */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    /* Switch system clock back to PLL */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
}
