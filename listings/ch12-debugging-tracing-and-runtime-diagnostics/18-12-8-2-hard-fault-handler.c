/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.8 Post-Mortem Analysis > 12.8.2 Hard Fault Handler
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
void BackupSRAM_Enable(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;   /* Backup SRAM clock         */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;       /* PWR clock                 */
    PWR->CR      |= PWR_CR_DBP;              /* Backup-domain write access */

    /* Only needed if contents must survive loss of VDD (VBAT supplied) */
    PWR->CSR |= PWR_CSR_BRE;
    /* Bounded wait: if BRR never sets, continue without VBAT retention */
    for (uint32_t t = 0; t < 100000U && !(PWR->CSR & PWR_CSR_BRR); t++) {}
}
