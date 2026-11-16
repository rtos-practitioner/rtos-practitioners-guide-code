/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.4 Timer Callback Rules and Restrictions > Rule 3: Delegate Long Work to a Worker Task
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static void vHeavyProcessingTimerCallback(TimerHandle_t xTimer)
{
    (void)xTimer;

    /* Signal the worker task -- do NOT process here.
       Timer callbacks run in the daemon task's context, so use the
       task-level API; xTaskNotifyGive() never blocks. */
    xTaskNotifyGive(xWorkerTaskHandle);
}
