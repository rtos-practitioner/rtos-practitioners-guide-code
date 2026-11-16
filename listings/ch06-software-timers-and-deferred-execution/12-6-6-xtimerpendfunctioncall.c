/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.6 Deferred Interrupt Processing Pattern > xTimerPendFunctionCall
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xTimerPendFunctionCall(
    PendedFunction_t xFunctionToPend,   /* Function to execute */
    void *pvParameter1,                 /* First parameter */
    uint32_t ulParameter2,              /* Second parameter */
    TickType_t xTicksToWait            /* Block time if queue full */
);

BaseType_t xTimerPendFunctionCallFromISR(
    PendedFunction_t xFunctionToPend,
    void *pvParameter1,
    uint32_t ulParameter2,
    BaseType_t *pxHigherPriorityTaskWoken
);
