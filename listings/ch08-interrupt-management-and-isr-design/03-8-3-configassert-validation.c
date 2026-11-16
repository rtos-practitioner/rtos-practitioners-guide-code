/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.3 FreeRTOS Interrupt Priority Model > configASSERT Validation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }
