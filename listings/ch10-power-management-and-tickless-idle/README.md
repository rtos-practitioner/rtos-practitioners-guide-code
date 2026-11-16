# Chapter 10: Power Management and Tickless Idle

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-10-2-sleep-mode.c](01-10-2-sleep-mode.c) | 10.2 STM32 Low-Power Modes > Sleep Mode | executed |
| [02-10-2-stop-mode.c](02-10-2-stop-mode.c) | 10.2 STM32 Low-Power Modes > Stop Mode | executed |
| [03-10-2-standby-mode.c](03-10-2-standby-mode.c) | 10.2 STM32 Low-Power Modes > Standby Mode | executed |
| [04-10-3-key-configuration-macros.c](04-10-3-key-configuration-macros.c) | 10.3 FreeRTOS Tickless Idle > Key Configuration Macros | executed (helper functions called with test arguments) |
| [05-FreeRTOSConfig.h](05-FreeRTOSConfig.h) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | compiled, linked and booted (declarations only) |
| [06-FreeRTOSConfig.h](06-FreeRTOSConfig.h) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | compiled, linked and booted (declarations only) |
| [07-10-4-step-by-step-implementation.c](07-10-4-step-by-step-implementation.c) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | executed |
| [08-10-4-step-by-step-implementation.c](08-10-4-step-by-step-implementation.c) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | compiled, linked and booted (declarations only) |
| [09-10-4-step-by-step-implementation.c](09-10-4-step-by-step-implementation.c) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | compiled, linked and booted (declarations only) |
| [10-10-4-step-by-step-implementation.c](10-10-4-step-by-step-implementation.c) | 10.4 Custom Tickless Implementation for Stop Mode > Step-by-Step Implementation | executed |
| [11-10-5-disabling-unused-peripheral-clocks.c](11-10-5-disabling-unused-peripheral-clocks.c) | 10.5 Peripheral Power Management > Disabling Unused Peripheral Clocks | executed |
| [12-10-5-gpio-power-optimization.c](12-10-5-gpio-power-optimization.c) | 10.5 Peripheral Power Management > GPIO Power Optimization | executed |
| [13-10-7-event-driven-architecture-bad.c](13-10-7-event-driven-architecture-bad.c) | 10.7 Design Patterns for Low Power > Event-Driven Architecture | executed |
| [14-10-7-communication-batching.c](14-10-7-communication-batching.c) | 10.7 Design Patterns for Low Power > Communication Batching | executed |
| [15-10-9-debug-probe-preventing-low-power-entry.c](15-10-9-debug-probe-preventing-low-power-entry.c) | 10.9 Common Pitfalls > Debug Probe Preventing Low-Power Entry | executed |
| [16-FreeRTOSConfig.h](16-FreeRTOSConfig.h) | 10.10 Lab 10: Tickless Idle Power Optimization > Part 1: Baseline Measurement (Standard Tick) | executed |
| [17-FreeRTOSConfig.h](17-FreeRTOSConfig.h) | 10.10 Lab 10: Tickless Idle Power Optimization > Part 2: Built-In Tickless Idle | compiled, linked and booted (declarations only) |
| [18-FreeRTOSConfig.h](18-FreeRTOSConfig.h) | 10.10 Lab 10: Tickless Idle Power Optimization > Part 3: Custom Stop-Mode Tickless | compiled, linked and booted (declarations only) |
| [19-10-10-part-3-custom-stop-mode-tickless.c](19-10-10-part-3-custom-stop-mode-tickless.c) | 10.10 Lab 10: Tickless Idle Power Optimization > Part 3: Custom Stop-Mode Tickless | executed |
