/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.2 STM32 Low-Power Modes > Sleep Mode
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Sleep mode entry */
SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  /* Clear SLEEPDEEP bit */
__WFI();                               /* Wait For Interrupt  */
