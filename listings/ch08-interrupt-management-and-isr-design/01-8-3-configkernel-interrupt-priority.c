/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.3 FreeRTOS Interrupt Priority Model > configKERNEL_INTERRUPT_PRIORITY
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configKERNEL_INTERRUPT_PRIORITY       ( 15 << 4 )  /* 0xF0 = lowest */
