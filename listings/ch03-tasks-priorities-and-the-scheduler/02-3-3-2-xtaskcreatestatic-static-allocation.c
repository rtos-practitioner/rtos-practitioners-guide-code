/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.3 Creating Tasks > 3.3.2 xTaskCreateStatic() — Static Allocation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
TaskHandle_t xTaskCreateStatic(
    TaskFunction_t pvTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void *pvParameters,
    UBaseType_t uxPriority,
    StackType_t * const puxStackBuffer,   /* Caller-provided stack array  */
    StaticTask_t * const pxTaskBuffer     /* Caller-provided TCB memory   */
);
