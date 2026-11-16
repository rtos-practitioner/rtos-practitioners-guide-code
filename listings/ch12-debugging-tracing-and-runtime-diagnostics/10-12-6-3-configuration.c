/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.6 SEGGER SystemView Integration > 12.6.3 Configuration
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define SEGGER_SYSVIEW_APP_NAME        "Motor Control Demo"
#define SEGGER_SYSVIEW_DEVICE_NAME     "STM32F407VG"
#define SEGGER_SYSVIEW_RAM_BASE        0x20000000
#define SEGGER_SYSVIEW_ID_BASE         0x20000000
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE 4096
