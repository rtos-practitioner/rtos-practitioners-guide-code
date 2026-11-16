/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.6 Context Switch Internals > 3.6.4 Measuring Context Switch Time
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Enable the DWT cycle counter */
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
DWT->CYCCNT = 0;
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

/* In a test task, record timestamp before yielding */
uint32_t t1 = DWT->CYCCNT;
taskYIELD();
/* After rescheduling, record timestamp */
uint32_t t2 = DWT->CYCCNT;

/* Context switch time (us) ≈ (t2 - t1) / 168  (168 cycles per us at 168 MHz) */
