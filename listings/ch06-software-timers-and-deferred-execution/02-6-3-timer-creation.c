/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.3 Creating and Managing Timers > Timer Creation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
TimerHandle_t xTimerCreate(
    const char * const pcTimerName,     /* Human-readable name (debug only) */
    TickType_t xTimerPeriodInTicks,     /* Period in ticks (must be > 0) */
    UBaseType_t uxAutoReload,           /* pdTRUE = auto-reload, pdFALSE = one-shot */
    void * pvTimerID,                   /* User-defined ID (often a pointer) */
    TimerCallbackFunction_t pxCallbackFunction  /* Function called on expiry */
);
