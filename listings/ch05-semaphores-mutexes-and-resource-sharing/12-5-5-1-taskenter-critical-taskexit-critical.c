/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.5 Critical Sections and Interrupt Masking > 5.5.1 taskENTER_CRITICAL / taskEXIT_CRITICAL
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
taskENTER_CRITICAL();
/* Access shared resource -- keep this VERY short */
g_shared_register |= ENABLE_BIT;
taskEXIT_CRITICAL();
