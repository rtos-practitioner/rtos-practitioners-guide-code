/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.8 Recovery Strategies > 13.8.5 Persistent Fault Counter
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
#define FAULT_COUNTER_ADDR  ((volatile uint32_t *)(BKPSRAM_BASE + 0x00))
#define FAULT_VALID_ADDR    ((volatile uint32_t *)(BKPSRAM_BASE + 0x04))
#define FAULT_VALID_MAGIC   0xB007C0DEU
#define MAX_REBOOT_ATTEMPTS 3

void check_reboot_limit(void)
{
    /* Enable backup SRAM clock and power */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR      |= PWR_CR_DBP;
    PWR->CSR     |= PWR_CSR_BRE;        /* Backup regulator for VBAT */
    for (uint32_t t = 0; t < 100000U && !(PWR->CSR & PWR_CSR_BRR); t++) {}
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;

    if (*FAULT_VALID_ADDR != FAULT_VALID_MAGIC) {
        /* First boot or backup domain lost — initialize */
        *FAULT_COUNTER_ADDR = 0;
        *FAULT_VALID_ADDR   = FAULT_VALID_MAGIC;
    }

    uint32_t count = *FAULT_COUNTER_ADDR;

    if (count >= MAX_REBOOT_ATTEMPTS) {
        /* Too many reboots — enter permanent safe state.
           Requires manual intervention (power cycle with
           service tool) to clear the counter. */
        safe_state();
    }

    *FAULT_COUNTER_ADDR = count + 1;

    /* Cleared later by clear_fault_counter() after a period of
       stable operation */
}

void clear_fault_counter(void)
{
    *FAULT_COUNTER_ADDR = 0;
}
