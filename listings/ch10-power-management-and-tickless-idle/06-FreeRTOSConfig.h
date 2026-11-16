/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h */
extern void App_EnterTicklessStop(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xIdleTicks) \
    App_EnterTicklessStop(xIdleTicks)
