/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.5 DWT Cycle Counter > 11.5.2 Enabling the Cycle Counter
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Enable DWT cycle counter */
CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* Enable trace     */
DWT->CYCCNT = 0;                                   /* Reset counter    */
DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              /* Start counting   */
