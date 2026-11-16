/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.1 The Critical Section Problem > 5.1.1 A Concrete Race: Read-Modify-Write
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Both Task A and Task B execute this */
g_count++;
