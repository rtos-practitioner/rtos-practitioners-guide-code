/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.5 DWT Cycle Counter > 11.5.3 Resolution and Overflow
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
uint32_t start = DWT->CYCCNT;
/* ... code under measurement ... */
uint32_t elapsed = DWT->CYCCNT - start;  /* Correct even if wrapped once */
