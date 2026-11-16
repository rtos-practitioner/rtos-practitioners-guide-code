/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.7 Event Groups > 5.7.3 Synchronization Rendezvous: xEventGroupSync
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
EventBits_t xEventGroupSync(EventGroupHandle_t xEventGroup,
                              const EventBits_t uxBitsToSet,
                              const EventBits_t uxBitsToWaitFor,
                              TickType_t xTicksToWait);
