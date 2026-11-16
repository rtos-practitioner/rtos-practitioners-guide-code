# Chapter 9: Device Driver Architecture and Peripheral Abstraction

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-9-2-1-layer-1-hardware-register-access.c](01-9-2-1-layer-1-hardware-register-access.c) | 9.2 Four-Layer Driver Model > 9.2.1 Layer 1: Hardware Register Access | compiled, linked and booted (declarations only) |
| [02-9-2-2-layer-2-vendor-hal.c](02-9-2-2-layer-2-vendor-hal.c) | 9.2 Four-Layer Driver Model > 9.2.2 Layer 2: Vendor HAL | executed |
| [03-9-2-4-layer-4-application-interface.c](03-9-2-4-layer-4-application-interface.c) | 9.2 Four-Layer Driver Model > 9.2.4 Layer 4: Application Interface | compiled, linked and booted (declarations only) |
| [04-uart_hw.h](04-uart_hw.h) | 9.3 UART Driver Case Study > 9.3.1 Layer 1: Register-Level Access | executed (helper functions called with test arguments) |
| [05-uart_hal.c](05-uart_hal.c) | 9.3 UART Driver Case Study > 9.3.2 Layer 2: Vendor HAL Initialization | executed (helper functions called with test arguments) |
| [06-uart_rtos.c](06-uart_rtos.c) | 9.3 UART Driver Case Study > 9.3.3 Layer 3: RTOS-Aware Driver | executed |
| [07-uart_api.h](07-uart_api.h) | 9.3 UART Driver Case Study > 9.3.4 Layer 4: Application Interface | compiled, linked and booted (declarations only) |
| [08-uart_api.c](08-uart_api.c) | 9.3 UART Driver Case Study > 9.3.4 Layer 4: Application Interface | executed (helper functions called with test arguments) |
| [09-spi_driver.h](09-spi_driver.h) | 9.4 SPI Driver Case Study > 9.4.2 Bus Sharing with Mutex | compiled, linked and booted (declarations only) |
| [10-spi_driver.c](10-spi_driver.c) | 9.4 SPI Driver Case Study > 9.4.3 Transaction-Based API | executed |
| [11-9-4-4-spi-flash-example.c](11-9-4-4-spi-flash-example.c) | 9.4 SPI Driver Case Study > 9.4.4 SPI Flash Example | executed (helper functions called with test arguments) |
| [12-i2c_driver.c](12-i2c_driver.c) | 9.5 I2C Driver Case Study > 9.5.2 Error Handling and Bus Recovery | executed (helper functions called with test arguments) |
| [13-9-5-3-application-level-i2c-api.c](13-9-5-3-application-level-i2c-api.c) | 9.5 I2C Driver Case Study > 9.5.3 Application-Level I2C API | executed (helper functions called with test arguments) |
| [14-9-5-4-temperature-sensor-example.c](14-9-5-4-temperature-sensor-example.c) | 9.5 I2C Driver Case Study > 9.5.4 Temperature Sensor Example | executed (helper functions called with test arguments) |
| [15-can_driver.h](15-can_driver.h) | 9.6 CAN Bus Driver > 9.6.2 FreeRTOS Integration | compiled, linked and booted (declarations only) |
| [16-can_driver.c](16-can_driver.c) | 9.6 CAN Bus Driver > 9.6.3 Implementation | executed (helper functions called with test arguments) |
| [17-uart_hal.c](17-uart_hal.c) | 9.7 The Fake HAL Pattern for Testing > 9.7.3 Fake HAL Implementation | executed (helper functions called with test arguments) |
| [18-test_command_parser.c](18-test_command_parser.c) | 9.7 The Fake HAL Pattern for Testing > 9.7.4 Test Example | executed |
| [19-9-8-1-singleton-pattern-with-handle-struct.c](19-9-8-1-singleton-pattern-with-handle-struct.c) | 9.8 Driver Design Patterns > 9.8.1 Singleton Pattern with Handle Struct | compiled, linked and booted (declarations only) |
| [20-9-8-2-callback-pattern.c](20-9-8-2-callback-pattern.c) | 9.8 Driver Design Patterns > 9.8.2 Callback Pattern | executed |
| [21-9-8-3-event-driven-pattern.c](21-9-8-3-event-driven-pattern.c) | 9.8 Driver Design Patterns > 9.8.3 Event-Driven Pattern | changed since the reference run - re-run tools/run_suite.py |
| [22-9-8-4-handle-pattern-with-opaque-pointers.c](22-9-8-4-handle-pattern-with-opaque-pointers.c) | 9.8 Driver Design Patterns > 9.8.4 Handle Pattern with Opaque Pointers | compiled, linked and booted (declarations only) |
| [23-9-9-1-qnx-resource-managers.c](23-9-9-1-qnx-resource-managers.c) | 9.9 QNX and Zephyr Portability Notes > 9.9.1 QNX: Resource Managers | not run (host command / other RTOS / configuration) |
| [24-9-9-2-zephyr-device-driver-model.c](24-9-9-2-zephyr-device-driver-model.c) | 9.9 QNX and Zephyr Portability Notes > 9.9.2 Zephyr: Device Driver Model | not run (host command / other RTOS / configuration) |
| [25-9-9-2-zephyr-device-driver-model.c](25-9-9-2-zephyr-device-driver-model.c) | 9.9 QNX and Zephyr Portability Notes > 9.9.2 Zephyr: Device Driver Model | not run (host command / other RTOS / configuration) |
| [26-main.c](26-main.c) | 9.11 Lab 9: Building a Reusable UART Driver > Part 1: Target Implementation | executed |
| [27-9-11-part-2-throughput-measurement.c](27-9-11-part-2-throughput-measurement.c) | 9.11 Lab 9: Building a Reusable UART Driver > Part 2: Throughput Measurement | executed (helper functions called with test arguments) |
| [28-9-11-part-3-fake-hal-testing-on-host.sh](28-9-11-part-3-fake-hal-testing-on-host.sh) | 9.11 Lab 9: Building a Reusable UART Driver > Part 3: Fake HAL Testing on Host | not run (host command / other RTOS / configuration) |
