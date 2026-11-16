/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.3 Watchdog Timers > 13.3.1 Independent Watchdog (IWDG)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void iwdg_init(void)
{
    IWDG->KR  = 0x5555;            /* Unlock PR and RLR registers   */
    IWDG->PR  = IWDG_PR_PR_2;      /* Prescaler /64                 */
    IWDG->RLR = 1000;              /* Reload = 1000                 */
    /* timeout = (64 * 1000) / 32000 = 2.0 seconds                  */

    while (IWDG->SR != 0) {}       /* Wait for registers to update  */
    IWDG->KR  = 0xAAAA;            /* Reload counter (first kick)   */
    IWDG->KR  = 0xCCCC;            /* Start IWDG — cannot be stopped */
}

void iwdg_refresh(void)
{
    IWDG->KR = 0xAAAA;             /* Write reload key              */
}
