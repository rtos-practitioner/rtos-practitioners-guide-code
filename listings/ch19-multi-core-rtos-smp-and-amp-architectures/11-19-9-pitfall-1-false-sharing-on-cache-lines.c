/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.9 Common Pitfalls > Pitfall 1: False Sharing on Cache Lines
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Give each shared variable its own 32-byte cache line */
__attribute__((aligned(32))) static uint32_t core0_counter;
__attribute__((aligned(32))) static uint32_t core1_counter;
