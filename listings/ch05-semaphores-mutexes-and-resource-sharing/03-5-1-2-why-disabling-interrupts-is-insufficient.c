/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.1 The Critical Section Problem > 5.1.2 Why Disabling Interrupts Is Insufficient
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
__disable_irq();
g_count++;
__enable_irq();
