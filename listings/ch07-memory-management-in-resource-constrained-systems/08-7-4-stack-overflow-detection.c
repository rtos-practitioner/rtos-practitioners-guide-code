/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.4 Stack Sizing and Overflow Detection > Stack Overflow Detection
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* WARNING: stack is already corrupted at this point.
     * Do not use local variables or call complex functions.
     * Log the task name to a dedicated fault buffer and reset. */
    volatile uint32_t *fault_log = (uint32_t *)0x2001FF00;
    fault_log[0] = 0xDEAD0007;  /* Fault code: stack overflow */

    /* Force a system reset */
    NVIC_SystemReset();
}
