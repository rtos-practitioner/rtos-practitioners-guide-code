/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.2 Binary Semaphores > 5.2.1 Creation and API
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,
                                  BaseType_t *pxHigherPriorityTaskWoken);
