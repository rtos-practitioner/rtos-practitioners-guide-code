/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.7 Runtime Diagnostics APIs > 12.7.2 vTaskGetRunTimeStats()
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* In FreeRTOSConfig.h */
#define configGENERATE_RUN_TIME_STATS           1
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1

/* Initialize DWT counter (call once at startup) */
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()            \
    do {                                                    \
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;    \
        DWT->CYCCNT = 0;                                    \
        DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;              \
    } while (0)

/* Read current count */
#define portGET_RUN_TIME_COUNTER_VALUE()  DWT->CYCCNT
