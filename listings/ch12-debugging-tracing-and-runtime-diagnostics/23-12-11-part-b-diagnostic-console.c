/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part B: Diagnostic Console
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
xTaskCreate(vConsoleTask, "Console", 1024, NULL, 1, NULL);
