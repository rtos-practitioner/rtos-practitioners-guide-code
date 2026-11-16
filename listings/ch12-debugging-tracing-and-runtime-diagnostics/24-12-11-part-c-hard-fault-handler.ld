/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part C: Hard Fault Handler
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
.backup_sram (NOLOAD) :
{
    *(.backup_sram)
} >BKPSRAM
