/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.3 Creating and Managing Timers > Timer Creation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
TimerHandle_t xTimerCreateStatic(
    const char * const pcTimerName,
    TickType_t xTimerPeriodInTicks,
    UBaseType_t uxAutoReload,
    void * pvTimerID,
    TimerCallbackFunction_t pxCallbackFunction,
    StaticTimer_t *pxTimerBuffer      /* Caller-allocated storage */
);
