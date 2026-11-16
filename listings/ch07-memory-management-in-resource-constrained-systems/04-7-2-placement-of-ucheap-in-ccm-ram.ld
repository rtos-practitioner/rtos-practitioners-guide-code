/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.2 FreeRTOS Heap Implementations > Placement of ucHeap in CCM RAM
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* In the linker script */
.ccmram (NOLOAD) :
{
    *(.ccmram)
} > CCMRAM
