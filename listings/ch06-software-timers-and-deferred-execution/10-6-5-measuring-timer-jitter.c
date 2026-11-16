/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.5 Timing Accuracy and Jitter > Measuring Timer Jitter
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static void vJitterTestCallback(TimerHandle_t xTimer)
{
    (void)xTimer;
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_0);  /* Measure this edge */
}
