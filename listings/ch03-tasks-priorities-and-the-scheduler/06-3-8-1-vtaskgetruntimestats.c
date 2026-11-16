/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.8 Runtime Statistics and Schedulability Analysis > 3.8.1 vTaskGetRunTimeStats()
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
#define configGENERATE_RUN_TIME_STATS           1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE()         ulGetRunTimeCounterValue()
