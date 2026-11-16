# Chapter 2: Development Environment, Project Layout, and Board Bring-Up

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-2-2-arm-gcc-toolchain.sh](01-2-2-arm-gcc-toolchain.sh) | 2.2 Toolchain Setup > ARM GCC Toolchain | not run (host command / other RTOS / configuration) |
| [02-2-4-phase-2-c-runtime-initialization-startup-assembly.c](02-2-4-phase-2-c-runtime-initialization-startup-assembly.c) | 2.4 The Boot Sequence Explained > Phase 2: C Runtime Initialization (Startup Assembly) | changed since the reference run - re-run tools/run_suite.py |
| [03-2-4-phase-3-system-initialization-and-main.c](03-2-4-phase-3-system-initialization-and-main.c) | 2.4 The Boot Sequence Explained > Phase 3: System Initialization and main() | executed |
| [04-2-7-scheduling-and-timing-configuration.c](04-2-7-scheduling-and-timing-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Scheduling and Timing Configuration | compiled, linked and booted (declarations only) |
| [05-2-7-scheduling-and-timing-configuration.c](05-2-7-scheduling-and-timing-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Scheduling and Timing Configuration | compiled, linked and booted (declarations only) |
| [06-2-7-scheduling-and-timing-configuration.c](06-2-7-scheduling-and-timing-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Scheduling and Timing Configuration | compiled, linked and booted (declarations only) |
| [07-2-7-scheduling-and-timing-configuration.c](07-2-7-scheduling-and-timing-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Scheduling and Timing Configuration | compiled, linked and booted (declarations only) |
| [08-2-7-resource-limits.c](08-2-7-resource-limits.c) | 2.7 FreeRTOSConfig.h Deep Dive > Resource Limits | compiled, linked and booted (declarations only) |
| [09-2-7-resource-limits.c](09-2-7-resource-limits.c) | 2.7 FreeRTOSConfig.h Deep Dive > Resource Limits | compiled, linked and booted (declarations only) |
| [10-2-7-resource-limits.c](10-2-7-resource-limits.c) | 2.7 FreeRTOSConfig.h Deep Dive > Resource Limits | compiled, linked and booted (declarations only) |
| [11-2-7-debug-and-safety-configuration.c](11-2-7-debug-and-safety-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Debug and Safety Configuration | compiled, linked and booted (declarations only) |
| [12-2-7-debug-and-safety-configuration.c](12-2-7-debug-and-safety-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Debug and Safety Configuration | compiled, linked and booted (declarations only) |
| [13-2-7-cortex-m-interrupt-configuration.c](13-2-7-cortex-m-interrupt-configuration.c) | 2.7 FreeRTOSConfig.h Deep Dive > Cortex-M Interrupt Configuration | compiled, linked and booted (declarations only) |
| [14-2-8-heartbeat-task-priority-2.c](14-2-8-heartbeat-task-priority-2.c) | 2.8 First Two Tasks > Heartbeat Task (Priority 2) | executed |
| [15-2-8-monitor-task-priority-4.c](15-2-8-monitor-task-priority-4.c) | 2.8 First Two Tasks > Monitor Task (Priority 4) | executed |
| [16-rtos_hooks.c](16-rtos_hooks.c) | 2.10 FreeRTOS Hook Functions | compiled, linked and booted (declarations only) |
| [17-2-13-zephyr-portability-note.sh](17-2-13-zephyr-portability-note.sh) | 2.13 Zephyr Portability Note | not run (host command / other RTOS / configuration) |
