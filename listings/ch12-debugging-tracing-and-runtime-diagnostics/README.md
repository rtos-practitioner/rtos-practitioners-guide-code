# Chapter 12: Debugging, Tracing, and Runtime Diagnostics

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-12-2-3-itm-instrumentation-trace-macrocell.c](01-12-2-3-itm-instrumentation-trace-macrocell.c) | 12.2 printf-Style Debugging > 12.2.3 ITM — Instrumentation Trace Macrocell | compiled, linked and booted (declarations only) |
| [02-12-2-4-segger-rtt-real-time-transfer.c](02-12-2-4-segger-rtt-real-time-transfer.c) | 12.2 printf-Style Debugging > 12.2.4 SEGGER RTT — Real-Time Transfer | executed (helper functions called with test arguments) |
| [03-flight_recorder.h](03-flight_recorder.h) | 12.4 FreeRTOS Trace Hooks > 12.4.2 The Flight Recorder Pattern | compiled, linked and booted (declarations only) |
| [04-flight_recorder.c](04-flight_recorder.c) | 12.4 FreeRTOS Trace Hooks > 12.4.2 The Flight Recorder Pattern | executed |
| [05-FreeRTOSConfig.h](05-FreeRTOSConfig.h) | 12.4 FreeRTOS Trace Hooks > 12.4.3 Hooking the Kernel | compiled, linked and booted (declarations only) |
| [06-12-4-4-gpio-toggle-for-oscilloscope-visualization.c](06-12-4-4-gpio-toggle-for-oscilloscope-visualization.c) | 12.4 FreeRTOS Trace Hooks > 12.4.4 GPIO Toggle for Oscilloscope Visualization | compiled, linked and booted (declarations only) |
| [07-trcConfig.h](07-trcConfig.h) | 12.5 Percepio Tracealyzer Integration > 12.5.3 Integration Steps | compiled, linked and booted (declarations only) |
| [08-12-5-3-integration-steps.c](08-12-5-3-integration-steps.c) | 12.5 Percepio Tracealyzer Integration > 12.5.3 Integration Steps | compiled, linked and booted (declarations only) |
| [09-12-5-3-integration-steps.c](09-12-5-3-integration-steps.c) | 12.5 Percepio Tracealyzer Integration > 12.5.3 Integration Steps | executed |
| [10-12-6-3-configuration.c](10-12-6-3-configuration.c) | 12.6 SEGGER SystemView Integration > 12.6.3 Configuration | compiled, linked and booted (declarations only) |
| [11-12-6-3-configuration.c](11-12-6-3-configuration.c) | 12.6 SEGGER SystemView Integration > 12.6.3 Configuration | executed |
| [12-12-6-5-custom-markers.c](12-12-6-5-custom-markers.c) | 12.6 SEGGER SystemView Integration > 12.6.5 Custom Markers | executed |
| [13-FreeRTOSConfig.h](13-FreeRTOSConfig.h) | 12.7 Runtime Diagnostics APIs > 12.7.2 vTaskGetRunTimeStats() | compiled, linked and booted (declarations only) |
| [14-12-7-3-uxtaskgetstackhighwatermark.c](14-12-7-3-uxtaskgetstackhighwatermark.c) | 12.7 Runtime Diagnostics APIs > 12.7.3 uxTaskGetStackHighWaterMark() | executed |
| [15-12-7-4-queue-monitoring.c](15-12-7-4-queue-monitoring.c) | 12.7 Runtime Diagnostics APIs > 12.7.4 Queue Monitoring | executed |
| [16-12-7-5-building-a-diagnostic-task.c](16-12-7-5-building-a-diagnostic-task.c) | 12.7 Runtime Diagnostics APIs > 12.7.5 Building a Diagnostic Task | executed |
| [17-12-8-2-hard-fault-handler.c](17-12-8-2-hard-fault-handler.c) | 12.8 Post-Mortem Analysis > 12.8.2 Hard Fault Handler | compiled, linked and booted (declarations only) |
| [18-12-8-2-hard-fault-handler.c](18-12-8-2-hard-fault-handler.c) | 12.8 Post-Mortem Analysis > 12.8.2 Hard Fault Handler | changed since the reference run - re-run tools/run_suite.py |
| [19-12-8-3-post-reset-recovery.c](19-12-8-3-post-reset-recovery.c) | 12.8 Post-Mortem Analysis > 12.8.3 Post-Reset Recovery | changed since the reference run - re-run tools/run_suite.py |
| [20-STM32F407VGTx_FLASH.ld](20-STM32F407VGTx_FLASH.ld) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part A: Flight Recorder | not run (host command / other RTOS / configuration) |
| [21-12-11-part-a-flight-recorder.c](21-12-11-part-a-flight-recorder.c) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part A: Flight Recorder | executed |
| [22-12-11-part-b-diagnostic-console.c](22-12-11-part-b-diagnostic-console.c) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part B: Diagnostic Console | executed |
| [23-12-11-part-b-diagnostic-console.c](23-12-11-part-b-diagnostic-console.c) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part B: Diagnostic Console | executed |
| [24-12-11-part-c-hard-fault-handler.ld](24-12-11-part-c-hard-fault-handler.ld) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part C: Hard Fault Handler | not run (host command / other RTOS / configuration) |
| [25-12-11-part-c-hard-fault-handler.ld](25-12-11-part-c-hard-fault-handler.ld) | 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part C: Hard Fault Handler | not run (host command / other RTOS / configuration) |
