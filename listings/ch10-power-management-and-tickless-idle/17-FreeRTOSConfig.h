/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.10 Lab 10: Tickless Idle Power Optimization > Part 2: Built-In Tickless Idle
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h */
#define configUSE_TICKLESS_IDLE              1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP  2
