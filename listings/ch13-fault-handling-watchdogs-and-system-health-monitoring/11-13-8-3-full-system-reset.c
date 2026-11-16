/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.8 Recovery Strategies > 13.8.3 Full System Reset
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void system_reset_with_log(uint32_t fault_code)
{
    __disable_irq();
    fault_log_to_backup_sram(fault_code, 0, 0, 0);
    NVIC_SystemReset();     /* Triggers SCB->AIRCR write */
}
