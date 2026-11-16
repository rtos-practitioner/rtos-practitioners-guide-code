/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.7 Advanced Patterns > Watchdog Architecture
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
#define NUM_MONITORED_TASKS  3
static volatile uint32_t ulTaskAliveFlags = 0;

/* Each monitored task sets its bit periodically */
void vTaskReportAlive(uint32_t ulTaskBit)
{
    /* Read-modify-write is not atomic -- protect it */
    taskENTER_CRITICAL();
    ulTaskAliveFlags |= ulTaskBit;
    taskEXIT_CRITICAL();
}

/* Auto-reload timer callback, period < watchdog timeout / 2 */
static void vWatchdogCheckCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    const uint32_t ulAllAlive = (1u << NUM_MONITORED_TASKS) - 1u;
    uint32_t ulSnapshot;

    /* Snapshot and clear together so no report is lost in between */
    taskENTER_CRITICAL();
    ulSnapshot = ulTaskAliveFlags;
    if ((ulSnapshot & ulAllAlive) == ulAllAlive) {
        ulTaskAliveFlags = 0;       /* Reset for next period */
    }
    taskEXIT_CRITICAL();

    if ((ulSnapshot & ulAllAlive) == ulAllAlive) {
        HAL_IWDG_Refresh(&hiwdg);  /* All tasks alive -- feed watchdog */
    }
    /* If any task missed its report, watchdog starves and MCU resets */
}
