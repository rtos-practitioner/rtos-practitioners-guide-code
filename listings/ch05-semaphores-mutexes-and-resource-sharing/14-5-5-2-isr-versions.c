/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.5 Critical Sections and Interrupt Masking > 5.5.2 ISR Versions
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void vMyISR(void)
{
    UBaseType_t uxSavedInterruptStatus;

    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    /* Short critical region */
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
}
