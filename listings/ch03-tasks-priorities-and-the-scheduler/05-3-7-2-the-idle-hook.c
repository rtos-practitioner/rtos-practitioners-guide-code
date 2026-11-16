/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.7 The Idle Task and Idle Hook > 3.7.2 The Idle Hook
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
void vApplicationIdleHook(void)
{
    /* Enter sleep mode; CPU wakes on next interrupt (SysTick or peripheral) */
    __WFI();
}
