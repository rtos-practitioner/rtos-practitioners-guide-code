/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.3 FreeRTOS Interrupt Priority Model > configMAX_SYSCALL_INTERRUPT_PRIORITY
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( 5 << 4 )   /* 0x50 */
