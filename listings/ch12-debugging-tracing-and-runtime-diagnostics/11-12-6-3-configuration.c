/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.6 SEGGER SystemView Integration > 12.6.3 Configuration
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include "SEGGER_SYSVIEW.h"

/* Call after NVIC and clock configuration */
SEGGER_SYSVIEW_Conf();
SEGGER_SYSVIEW_Start();
