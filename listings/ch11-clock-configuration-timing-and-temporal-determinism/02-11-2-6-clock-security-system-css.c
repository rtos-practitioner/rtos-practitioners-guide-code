/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.2 STM32F407 Clock Architecture > 11.2.6 Clock Security System (CSS)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void NMI_Handler(void) {
    if (RCC->CIR & RCC_CIR_CSSF) {
        RCC->CIR |= RCC_CIR_CSSC;      /* Clear CSS flag           */
        enter_safe_state();              /* Application-defined      */
    }
}
