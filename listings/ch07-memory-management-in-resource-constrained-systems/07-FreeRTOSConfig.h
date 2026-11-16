/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.3 Static vs Dynamic Allocation > Example: Fully Static System with Zero Heap
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h */
#define configSUPPORT_STATIC_ALLOCATION     1
#define configSUPPORT_DYNAMIC_ALLOCATION    0
/* configTOTAL_HEAP_SIZE is not needed---no heap_N.c file is compiled */
