/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 2: Development Environment, Project Layout, and Board Bring-Up
 * 2.10 FreeRTOS Hook Functions
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* rtos_hooks.c - FreeRTOS callback implementations */

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    /* Log or signal the fault */
    printf("STACK OVERFLOW: %s\r\n", pcTaskName);
    /* Enter safe state */
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

void vApplicationMallocFailedHook(void) {
    printf("MALLOC FAILED\r\n");
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

void vApplicationIdleHook(void) {
    /* Optional: enter low-power mode or collect statistics */
}

void vApplicationTickHook(void) {
    /* Optional: called from tick interrupt context */
}
