/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.5 Peripheral Power Management > GPIO Power Optimization
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Configure unused pins as analog to minimize leakage */
GPIOC->MODER = 0xFFFFFFFF;  /* All pins analog mode (binary 11 per pin) */
