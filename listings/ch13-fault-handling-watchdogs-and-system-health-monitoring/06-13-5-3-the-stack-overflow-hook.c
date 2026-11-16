/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.5 Stack Overflow Detection and Response > 13.5.3 The Stack Overflow Hook
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName)
{
    /* WARNING: stack is already corrupted — be minimal here.
       Do NOT call printf, queue operations, or any function
       that allocates stack frames larger than the remaining
       headroom. */

    __disable_irq();

    /* Write fault record to backup SRAM (no stack needed) */
    volatile uint32_t *bkp = (volatile uint32_t *)BKPSRAM_BASE;
    bkp[0] = 0xDEAD57AC;                /* Magic marker        */
    bkp[1] = (uint32_t)xTask;           /* Faulting task handle */
    bkp[2] = (uint32_t)pcTaskName;      /* Task name pointer   */

    /* Enter safe state and reset */
    safe_state();
}
