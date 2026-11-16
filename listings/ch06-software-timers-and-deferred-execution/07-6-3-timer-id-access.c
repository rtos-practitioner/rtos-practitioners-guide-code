/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.3 Creating and Managing Timers > Timer ID Access
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
void *pvTimerGetTimerID(TimerHandle_t xTimer);
void  vTimerSetTimerID(TimerHandle_t xTimer, void *pvNewID);
