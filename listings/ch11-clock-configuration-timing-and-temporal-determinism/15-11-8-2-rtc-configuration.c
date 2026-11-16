/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.8 Real-Time Clock (RTC) for Calendar Time > 11.8.2 RTC Configuration
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void RTC_Init(void) {
    /* Enable PWR clock and access to backup domain */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    /* Select LSE as RTC clock source */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));    /* Wait for LSE ready */
    RCC->BDCR |= RCC_BDCR_RTCSEL_0;            /* LSE = RTC source   */
    RCC->BDCR |= RCC_BDCR_RTCEN;               /* Enable RTC         */

    /* Unlock RTC write protection */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* Enter init mode */
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF));

    /* Prescaler: async = 127, sync = 255 => 32768 / (128 * 256) = 1 Hz */
    RTC->PRER = (127UL << 16) | 255UL;

    /* Set time: 14:30:00 (BCD) */
    RTC->TR = (0x1UL << 20) | (0x4UL << 16)    /* Hours: 14          */
            | (0x3UL << 12) | (0x0UL << 8)      /* Minutes: 30        */
            | (0x0UL << 4)  | (0x0UL << 0);     /* Seconds: 00        */

    /* Exit init mode */
    RTC->ISR &= ~RTC_ISR_INIT;

    /* Lock RTC write protection */
    RTC->WPR = 0xFF;
}
