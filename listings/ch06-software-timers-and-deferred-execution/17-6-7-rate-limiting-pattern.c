/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.7 Advanced Patterns > Rate-Limiting Pattern
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static TimerHandle_t xRateLimitTimer;
static volatile BaseType_t xActionPermitted = pdTRUE;

static void vRateLimitCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    xActionPermitted = pdTRUE;  /* Re-enable the action */
}

void vRateLimitedAction(void)
{
    if (xActionPermitted == pdTRUE) {
        xActionPermitted = pdFALSE;
        /* Perform the action */
        vSendErrorReport();
        /* Start one-shot timer to re-enable after minimum interval */
        xTimerStart(xRateLimitTimer, 0);
    }
    /* else: action suppressed -- too soon since last execution */
}
