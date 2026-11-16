/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.9 Common Pitfalls > Debug Probe Preventing Low-Power Entry
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Disable debug in low-power modes for accurate power measurement */
DBGMCU->CR &= ~(DBGMCU_CR_DBG_SLEEP |
                 DBGMCU_CR_DBG_STOP  |
                 DBGMCU_CR_DBG_STANDBY);
