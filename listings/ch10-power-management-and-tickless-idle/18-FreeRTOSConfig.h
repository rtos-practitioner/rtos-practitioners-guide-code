/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.10 Lab 10: Tickless Idle Power Optimization > Part 3: Custom Stop-Mode Tickless
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h */
#define configUSE_TICKLESS_IDLE    2
extern void App_EnterTicklessStop(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(x)  App_EnterTicklessStop(x)
