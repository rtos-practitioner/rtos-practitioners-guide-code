/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.5 Timing Accuracy and Jitter > Measuring Timer Jitter
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static volatile uint32_t ulLastCallbackCycle = 0;
static volatile uint32_t ulMaxJitterCycles = 0;
static const uint32_t ulExpectedPeriodCycles = 168000 * 100; /* 100ms at 168MHz */

static void vJitterMeasureCallback(TimerHandle_t xTimer)
{
    uint32_t ulNow = DWT->CYCCNT;
    if (ulLastCallbackCycle != 0) {
        uint32_t ulActualPeriod = ulNow - ulLastCallbackCycle;
        int32_t lDeviation = (int32_t)(ulActualPeriod - ulExpectedPeriodCycles);
        uint32_t ulAbsDev = (lDeviation < 0) ? -lDeviation : lDeviation;
        if (ulAbsDev > ulMaxJitterCycles) {
            ulMaxJitterCycles = ulAbsDev;
        }
    }
    ulLastCallbackCycle = ulNow;
}
