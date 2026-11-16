/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.6 Interrupt Nesting > Worst-Case Stack Calculation
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* In STM32F407VGTx_FLASH.ld */
_Min_Stack_Size = 0x800;  /* 2048 bytes -- conservative for nesting */
