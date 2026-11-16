/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.5 DWT Cycle Counter > 11.5.4 Conversion Macros
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define DWT_CYCLES_TO_US(cycles)  ((uint32_t)((uint64_t)(cycles) * 1000000ULL \
                                   / configCPU_CLOCK_HZ))
#define DWT_CYCLES_TO_NS(cycles)  ((uint32_t)((uint64_t)(cycles) * 1000000000ULL \
                                   / configCPU_CLOCK_HZ))
#define DWT_US_TO_CYCLES(us)      ((uint32_t)((uint64_t)(us) \
                                   * configCPU_CLOCK_HZ / 1000000ULL))
