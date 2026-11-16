/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.3 FreeRTOS SMP > 19.3.1 Configuration
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Enable SMP with 2 cores (e.g., RP2040, ESP32-S3) */
#define configNUMBER_OF_CORES    2

/* Allow tasks of different priorities to run at the same time
   on different cores (0 = only equal-priority tasks run together) */
#define configRUN_MULTIPLE_PRIORITIES    1

/* Enable core affinity API */
#define configUSE_CORE_AFFINITY    1
