/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.2 STM32 Low-Power Modes > Standby Mode
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Standby mode entry */
SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
PWR->CR  |= PWR_CR_PDDS;              /* Select Standby         */
PWR->CR  |= PWR_CR_CWUF;              /* Clear wakeup flag      */
__WFI();                               /* Enter Standby mode     */
/* Processor resets on wakeup --- execution does NOT continue here */
