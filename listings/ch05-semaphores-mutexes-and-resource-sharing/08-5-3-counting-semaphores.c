/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.3 Counting Semaphores
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount,
                                            UBaseType_t uxInitialCount);
