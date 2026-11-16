/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part A: Flight Recorder
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void USART2_IRQHandler(void)
{
    FR_Record(FR_EVT_ISR_ENTER, 0, 38);  /* IRQ number 38 */
    HAL_UART_IRQHandler(&huart2);
    FR_Record(FR_EVT_ISR_EXIT,  0, 38);
}
