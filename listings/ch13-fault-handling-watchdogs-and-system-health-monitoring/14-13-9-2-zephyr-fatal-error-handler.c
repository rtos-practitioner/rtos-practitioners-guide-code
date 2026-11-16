/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.9 QNX and Zephyr: Fault Management Approaches > 13.9.2 Zephyr Fatal Error Handler
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void k_sys_fatal_error_handler(unsigned int reason,
                               const struct arch_esf *esf)
{
    LOG_PANIC();                        /* Flush any buffered log data */
    save_crash_dump(reason, esf);       /* Persist to retained memory  */
    safe_state();
    sys_reboot(SYS_REBOOT_COLD);
}
