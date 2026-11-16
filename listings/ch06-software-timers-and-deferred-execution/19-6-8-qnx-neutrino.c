/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.8 QNX and Zephyr Portability Notes > QNX Neutrino
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* QNX POSIX timer with pulse delivery */
struct sigevent event;
timer_t timer_id;
struct itimerspec itime;

SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT,
                 TIMER_PULSE_CODE, 0);
timer_create(CLOCK_MONOTONIC, &event, &timer_id);
itime.it_value.tv_sec = 0;
itime.it_value.tv_nsec = 100000000;  /* 100 ms */
itime.it_interval = itime.it_value;   /* Auto-reload */
timer_settime(timer_id, 0, &itime, NULL);
