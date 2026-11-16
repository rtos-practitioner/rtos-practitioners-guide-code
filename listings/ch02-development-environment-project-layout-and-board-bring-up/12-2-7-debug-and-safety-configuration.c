/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 2: Development Environment, Project Layout, and Board Bring-Up
 * 2.7 FreeRTOSConfig.h Deep Dive > Debug and Safety Configuration
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configASSERT(x)  if (!(x)) { taskDISABLE_INTERRUPTS(); for(;;); }
