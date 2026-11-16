/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.6 Fragmentation Analysis and Prevention > Detection
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
size_t xFreeNow = xPortGetFreeHeapSize();
size_t xFreeMin = xPortGetMinimumEverFreeHeapSize();
