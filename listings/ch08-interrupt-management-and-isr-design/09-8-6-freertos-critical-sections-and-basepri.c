/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.6 Interrupt Nesting > FreeRTOS Critical Sections and BASEPRI
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* Simplified from portmacro.h for Cortex-M4 */
static inline void vPortRaiseBASEPRI(void)
{
    __asm volatile(
        "mov r0, %0      \n"
        "msr basepri, r0  \n"
        ::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY)
        :"r0"
    );
}
