/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.7 Temporal Determinism Strategies > 11.7.3 Interrupt Management for Determinism
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
taskENTER_CRITICAL();   /* Masks interrupts up to configMAX_SYSCALL_INTERRUPT_PRIORITY */
/* ... deterministic code, max ~10 us recommended ... */
taskEXIT_CRITICAL();
