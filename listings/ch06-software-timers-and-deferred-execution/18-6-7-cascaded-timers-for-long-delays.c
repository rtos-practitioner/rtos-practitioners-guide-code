/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.7 Advanced Patterns > Cascaded Timers for Long Delays
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static uint32_t ulRemainingMinutes = 0;

static void vLongDelayCallback(TimerHandle_t xTimer)
{
    if (ulRemainingMinutes > 0) {
        ulRemainingMinutes--;
        xTimerStart(xTimer, 0);  /* Restart for next minute */
    } else {
        vLongDelayExpired();      /* Final expiration */
    }
}

void vStartLongDelay(uint32_t ulMinutes)
{
    ulRemainingMinutes = ulMinutes - 1;
    xTimerChangePeriod(xLongDelayTimer, pdMS_TO_TICKS(60000), portMAX_DELAY);
}
