/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.3 Creating and Managing Timers > Starting, Stopping, and Modifying Timers
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Start a timer (or restart if already running) */
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait);

/* Stop a running timer */
BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait);

/* Reset a timer: restarts countdown from now */
BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait);

/* Change the period (also starts the timer if dormant) */
BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,
                               TickType_t xNewPeriod,
                               TickType_t xTicksToWait);
