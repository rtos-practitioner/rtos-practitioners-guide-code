/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.4 Hardware Timers for Precise Timing > 11.4.4 Output Compare Mode
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Schedule an event exactly 500 us from now using TIM2 output compare */
uint32_t target = TIM2->CNT + (84 * 500);  /* 84 counts/us * 500 us */
TIM2->CCR2 = target;                        /* Load compare register  */
TIM2->DIER |= TIM_DIER_CC2IE;              /* Enable compare interrupt*/
