# Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-13-2-1-the-four-fault-types.c](01-13-2-1-the-four-fault-types.c) | 13.2 ARM Cortex-M Fault Handlers > 13.2.1 The Four Fault Types | executed |
| [02-13-2-3-extracting-the-stacked-pc.c](02-13-2-3-extracting-the-stacked-pc.c) | 13.2 ARM Cortex-M Fault Handlers > 13.2.3 Extracting the Stacked PC | executed (helper functions called with test arguments) |
| [03-13-3-1-independent-watchdog-iwdg.c](03-13-3-1-independent-watchdog-iwdg.c) | 13.3 Watchdog Timers > 13.3.1 Independent Watchdog (IWDG) | executed |
| [04-13-3-2-window-watchdog-wwdg.c](04-13-3-2-window-watchdog-wwdg.c) | 13.3 Watchdog Timers > 13.3.2 Window Watchdog (WWDG) | executed |
| [05-13-4-2-implementation.c](05-13-4-2-implementation.c) | 13.4 Task-Level Health Monitoring > 13.4.2 Implementation | executed |
| [06-13-5-3-the-stack-overflow-hook.c](06-13-5-3-the-stack-overflow-hook.c) | 13.5 Stack Overflow Detection and Response > 13.5.3 The Stack Overflow Hook | compiled, linked and booted (declarations only) |
| [07-FreeRTOSConfig.h](07-FreeRTOSConfig.h) | 13.6 Assertion Framework > 13.6.1 configASSERT Implementation | executed (helper functions called with test arguments) |
| [08-13-7-2-implementation.c](08-13-7-2-implementation.c) | 13.7 Safe-State Design > 13.7.2 Implementation | compiled, linked and booted (declarations only) |
| [09-13-7-3-protected-memory-placement.ld](09-13-7-3-protected-memory-placement.ld) | 13.7 Safe-State Design > 13.7.3 Protected Memory Placement | not run (host command / other RTOS / configuration) |
| [10-13-8-1-task-restart.c](10-13-8-1-task-restart.c) | 13.8 Recovery Strategies > 13.8.1 Task Restart | executed (helper functions called with test arguments) |
| [11-13-8-3-full-system-reset.c](11-13-8-3-full-system-reset.c) | 13.8 Recovery Strategies > 13.8.3 Full System Reset | executed (helper functions called with test arguments) |
| [12-13-8-4-reset-cause-identification.c](12-13-8-4-reset-cause-identification.c) | 13.8 Recovery Strategies > 13.8.4 Reset Cause Identification | executed (helper functions called with test arguments) |
| [13-13-8-5-persistent-fault-counter.c](13-13-8-5-persistent-fault-counter.c) | 13.8 Recovery Strategies > 13.8.5 Persistent Fault Counter | changed since the reference run - re-run tools/run_suite.py |
| [14-13-9-2-zephyr-fatal-error-handler.c](14-13-9-2-zephyr-fatal-error-handler.c) | 13.9 QNX and Zephyr: Fault Management Approaches > 13.9.2 Zephyr Fatal Error Handler | executed (helper functions called with test arguments) |
| [15-lab13_health_monitor.c](15-lab13_health_monitor.c) | 13.11 Lab 13: Health Monitor with Crash Logging > 13.11.2 Complete Implementation | changed since the reference run - re-run tools/run_suite.py |
