/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.7 Advanced Patterns > Timeout Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static TimerHandle_t xCommTimeoutTimer;

static void vCommTimeoutCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    /* Communication did not complete in time */
    vCommAbort();
    xEventGroupSetBits(xCommEvents, COMM_TIMEOUT_BIT);
}

void vStartCommunication(void)
{
    /* Begin the operation */
    vCommBeginTransfer();
    /* Arm the timeout */
    xTimerStart(xCommTimeoutTimer, portMAX_DELAY);
}

void vOnCommunicationComplete(void)
{
    /* Operation succeeded -- cancel the timeout */
    xTimerStop(xCommTimeoutTimer, portMAX_DELAY);
    xEventGroupSetBits(xCommEvents, COMM_COMPLETE_BIT);
}
