/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.2 ARM Cortex-M Fault Handlers > 13.2.1 The Four Fault Types
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Enable MemManage, BusFault, and UsageFault exceptions */
SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk    /* bit 16 */
            | SCB_SHCSR_BUSFAULTENA_Msk     /* bit 17 */
            | SCB_SHCSR_USGFAULTENA_Msk;    /* bit 18 */

/* Enable divide-by-zero and unaligned access traps */
SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk           /* bit 4 */
           | SCB_CCR_UNALIGN_TRP_Msk;        /* bit 3 */
