/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.4 FreeRTOS Trace Hooks > 12.4.4 GPIO Toggle for Oscilloscope Visualization
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define traceTASK_SWITCHED_IN()                                       \
    do {                                                              \
        if (pxCurrentTCB->uxTaskNumber == MOTOR_TASK_ID)              \
            GPIOD->BSRR = GPIO_PIN_12;    /* PD12 high */            \
        else                                                          \
            GPIOD->BSRR = GPIO_PIN_12 << 16; /* PD12 low */          \
    } while (0)
