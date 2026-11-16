/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.10 Common Pitfalls
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
uint8_t ucDmaBuf[256] __attribute__((section(".sram")));
