/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.3 FreeRTOS SMP > 19.3.3 Scheduler and Synchronization Changes
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* SMP critical section — same API as single-core FreeRTOS */
taskENTER_CRITICAL();
/* Local interrupts masked AND the kernel spinlocks held,
   so no other core can be inside a critical section */
shared_counter++;
taskEXIT_CRITICAL();
