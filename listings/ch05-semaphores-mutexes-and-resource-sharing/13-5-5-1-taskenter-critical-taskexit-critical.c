/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.5 Critical Sections and Interrupt Masking > 5.5.1 taskENTER_CRITICAL / taskEXIT_CRITICAL
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
taskENTER_CRITICAL();       /* nesting = 1, BASEPRI set */
    taskENTER_CRITICAL();   /* nesting = 2, BASEPRI unchanged */
    taskEXIT_CRITICAL();    /* nesting = 1, BASEPRI unchanged */
taskEXIT_CRITICAL();        /* nesting = 0, BASEPRI restored */
