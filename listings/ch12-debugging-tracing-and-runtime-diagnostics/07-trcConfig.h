/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.5 Percepio Tracealyzer Integration > 12.5.3 Integration Steps
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* trcConfig.h key settings */
#define TRC_CFG_RECORDER_MODE       TRC_RECORDER_MODE_STREAMING
#define TRC_CFG_FREERTOS_VERSION    TRC_FREERTOS_VERSION_10_5_1
#define TRC_CFG_SCHEDULING_ONLY     0  /* 0 = full trace, 1 = task switches only */
#define TRC_CFG_INCLUDE_USER_EVENTS 1
