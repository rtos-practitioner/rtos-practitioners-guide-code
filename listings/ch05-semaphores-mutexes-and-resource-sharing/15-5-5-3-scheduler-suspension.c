/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.5 Critical Sections and Interrupt Masking > 5.5.3 Scheduler Suspension
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
vTaskSuspendAll();
/* No context switch can occur here, but interrupts still fire */
/* Modify task-only shared data */
xTaskResumeAll();
