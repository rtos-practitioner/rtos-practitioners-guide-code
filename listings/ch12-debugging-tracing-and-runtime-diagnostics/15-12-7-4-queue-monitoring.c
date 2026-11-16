/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.7 Runtime Diagnostics APIs > 12.7.4 Queue Monitoring
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
UBaseType_t waiting = uxQueueMessagesWaiting(xSensorQueue);
UBaseType_t spaces  = uxQueueSpacesAvailable(xSensorQueue);
