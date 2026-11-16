/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.2 printf-Style Debugging > 12.2.4 SEGGER RTT — Real-Time Transfer
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
#include "SEGGER_RTT.h"

void vLogEvent(const char *msg)
{
    SEGGER_RTT_printf(0, "[%10u] %s\n",
                      (unsigned)DWT->CYCCNT, msg);
}
