# Chapter 14: Testing Real-Time Systems

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-CMakeLists.txt](01-CMakeLists.txt) | 14.2 Unit Testing on Host > Dual Build Targets | not run (host command / other RTOS / configuration) |
| [02-14-2-dual-build-targets.sh](02-14-2-dual-build-targets.sh) | 14.2 Unit Testing on Host > Dual Build Targets | not run (host command / other RTOS / configuration) |
| [03-14-2-dual-build-targets.sh](03-14-2-dual-build-targets.sh) | 14.2 Unit Testing on Host > Dual Build Targets | not run (host command / other RTOS / configuration) |
| [04-moving_average.h](04-moving_average.h) | 14.2 Unit Testing on Host > Code Example: Unity Test for a Moving Average Filter | compiled, linked and booted (declarations only) |
| [05-test_moving_average.c](05-test_moving_average.c) | 14.2 Unit Testing on Host > Code Example: Unity Test for a Moving Average Filter | executed |
| [06-test_motor_sm.c](06-test_motor_sm.c) | 14.2 Unit Testing on Host > Code Example: Testing a State Machine with Stub Inputs | executed |
| [07-fake_hal.c](07-fake_hal.c) | 14.3 The Fake HAL Pattern (Expanded) > Simulating Timing | executed (helper functions called with test arguments) |
| [08-14-3-simulating-timing.c](08-14-3-simulating-timing.c) | 14.3 The Fake HAL Pattern (Expanded) > Simulating Timing | executed (helper functions called with test arguments) |
| [09-fake_i2c_hal.c](09-fake_i2c_hal.c) | 14.3 The Fake HAL Pattern (Expanded) > Code Example: Fake I2C HAL with Programmable Responses | executed (helper functions called with test arguments) |
| [10-14-3-code-example-fake-i2c-hal-with-programmable-responses.c](10-14-3-code-example-fake-i2c-hal-with-programmable-responses.c) | 14.3 The Fake HAL Pattern (Expanded) > Code Example: Fake I2C HAL with Programmable Responses | executed (helper functions called with test arguments) |
| [11-test_pipeline.c](11-test_pipeline.c) | 14.4 Integration Testing > Code Example: Integration Test on FreeRTOS Simulator | executed |
| [12-14-5-test-harness-design.sh](12-14-5-test-harness-design.sh) | 14.5 System Testing on Target > Test Harness Design | not run (host command / other RTOS / configuration) |
| [13-14-5-test-harness-design.c](13-14-5-test-harness-design.c) | 14.5 System Testing on Target > Test Harness Design | compiled, linked and booted (declarations only) |
| [14-14-5-timing-verification.c](14-14-5-timing-verification.c) | 14.5 System Testing on Target > Timing Verification | executed (helper functions called with test arguments) |
| [15-test_temperature_alarm.py](15-test_temperature_alarm.py) | 14.6 Hardware-in-the-Loop (HIL) Testing > Scripted Test Sequences | not run (host command / other RTOS / configuration) |
| [16-14-7-coverage-on-host-build.sh](16-14-7-coverage-on-host-build.sh) | 14.7 Code Coverage for Embedded > Coverage on Host Build | not run (host command / other RTOS / configuration) |
| [17-14-8-automated-flash-and-test.sh](17-14-8-automated-flash-and-test.sh) | 14.8 Continuous Integration for Embedded > Automated Flash and Test | not run (host command / other RTOS / configuration) |
| [18-firmware-ci.yml](18-firmware-ci.yml) | 14.8 Continuous Integration for Embedded > GitHub Actions Example | not run (host command / other RTOS / configuration) |
| [19-circular_buffer.h](19-circular_buffer.h) | 14.11 Lab 14: Building a Test Pipeline > Part 1: Unit Tests for a Circular Buffer | compiled, linked and booted (declarations only) |
| [20-test_circular_buffer.c](20-test_circular_buffer.c) | 14.11 Lab 14: Building a Test Pipeline > Part 1: Unit Tests for a Circular Buffer | executed |
| [21-fake_uart_hal.c](21-fake_uart_hal.c) | 14.11 Lab 14: Building a Test Pipeline > Part 2: Fake UART HAL | executed (helper functions called with test arguments) |
| [22-CMakeLists.txt](22-CMakeLists.txt) | 14.11 Lab 14: Building a Test Pipeline > Part 3: FreeRTOS Simulator Integration Test | not run (host command / other RTOS / configuration) |
| [23-target_test_harness.c](23-target_test_harness.c) | 14.11 Lab 14: Building a Test Pipeline > Part 4: On-Target Test Harness | changed since the reference run - re-run tools/run_suite.py |
