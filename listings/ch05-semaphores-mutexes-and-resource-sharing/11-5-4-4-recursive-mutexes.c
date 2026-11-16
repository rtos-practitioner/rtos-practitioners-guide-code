/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.4 Mutexes and Priority Inheritance > 5.4.4 Recursive Mutexes
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void);

BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t xMutex,
                                    TickType_t xTicksToWait);

BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t xMutex);
