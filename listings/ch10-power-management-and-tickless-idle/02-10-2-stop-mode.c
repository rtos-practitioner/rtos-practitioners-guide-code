/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.2 STM32 Low-Power Modes > Stop Mode
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Stop mode entry with voltage regulator in low-power mode */
SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;           /* Set SLEEPDEEP bit     */
PWR->CR  &= ~PWR_CR_PDDS;                     /* Select Stop (not Standby) */
PWR->CR  |= PWR_CR_LPDS;                      /* Regulator low-power   */
__WFI();                                       /* Enter Stop mode       */

/* --- Processor wakes here --- */
SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           /* Clear for normal sleep */
