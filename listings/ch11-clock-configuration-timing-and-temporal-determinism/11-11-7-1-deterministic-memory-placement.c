/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.7 Temporal Determinism Strategies > 11.7.1 Deterministic Memory Placement
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Place variable in CCM using GCC section attribute */
__attribute__((section(".ccmram")))
static float pid_coefficients[3];

/* Linker script excerpt for CCM:
 * .ccmram : {
 *     . = ALIGN(4);
 *     *(.ccmram)
 *     . = ALIGN(4);
 * } > CCMRAM
 */
