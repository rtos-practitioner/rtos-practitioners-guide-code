/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.10 Lab 7: Memory Budget and Overflow Detection > FreeRTOSConfig.h Settings
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configTOTAL_HEAP_SIZE           ((size_t)(40 * 1024))  /* 40 KB */
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_TRACE_FACILITY        1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configRECORD_STACK_HIGH_ADDRESS  1
