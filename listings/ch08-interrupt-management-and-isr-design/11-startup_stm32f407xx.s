/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.6 Interrupt Nesting > Worst-Case Stack Calculation
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
; In startup_stm32f407xx.s
Stack_Size      EQU     0x00000800    ; 2048 bytes
