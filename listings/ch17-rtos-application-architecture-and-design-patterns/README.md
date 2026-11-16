# Chapter 17: RTOS Application Architecture and Design Patterns

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-17-3-1-events-as-first-class-entities.c](01-17-3-1-events-as-first-class-entities.c) | 17.3 Event-Driven Architecture > 17.3.1 Events as First-Class Entities | compiled, linked and booted (declarations only) |
| [02-17-3-2-event-loop-pattern.c](02-17-3-2-event-loop-pattern.c) | 17.3 Event-Driven Architecture > 17.3.2 Event Loop Pattern | executed |
| [03-17-3-3-centralized-event-dispatcher.c](03-17-3-3-centralized-event-dispatcher.c) | 17.3 Event-Driven Architecture > 17.3.3 Centralized Event Dispatcher | executed |
| [04-17-4-3-code-example-led-controller-active-object.c](04-17-4-3-code-example-led-controller-active-object.c) | 17.4 Active Object Pattern > 17.4.3 Code Example: LED Controller Active Object | executed |
| [05-17-5-2-code-example-sensor-data-pub-sub-system.c](05-17-5-2-code-example-sensor-data-pub-sub-system.c) | 17.5 Publish-Subscribe Pattern > 17.5.2 Code Example: Sensor Data Pub-Sub System | executed (helper functions called with test arguments) |
| [06-17-6-1-command-descriptor.c](06-17-6-1-command-descriptor.c) | 17.6 Command Pattern and Protocol Handlers > 17.6.1 Command Descriptor | compiled, linked and booted (declarations only) |
| [07-17-6-2-command-table-and-dispatcher.c](07-17-6-2-command-table-and-dispatcher.c) | 17.6 Command Pattern and Protocol Handlers > 17.6.2 Command Table and Dispatcher | executed (helper functions called with test arguments) |
| [08-17-6-3-serial-command-processor-task.c](08-17-6-3-serial-command-processor-task.c) | 17.6 Command Pattern and Protocol Handlers > 17.6.3 Serial Command Processor Task | executed |
| [09-17-7-1-simple-switch-case-state-machine.c](09-17-7-1-simple-switch-case-state-machine.c) | 17.7 State Machine Patterns > 17.7.1 Simple Switch-Case State Machine | compiled, linked and booted (declarations only) |
| [10-17-7-2-table-driven-state-machine.c](10-17-7-2-table-driven-state-machine.c) | 17.7 State Machine Patterns > 17.7.2 Table-Driven State Machine | executed (helper functions called with test arguments) |
| [11-17-8-1-raii-like-macros-in-c.c](11-17-8-1-raii-like-macros-in-c.c) | 17.8 Resource Management Patterns > 17.8.1 RAII-Like Macros in C | executed (helper functions called with test arguments) |
| [12-17-8-2-spi-bus-broker-task.c](12-17-8-2-spi-bus-broker-task.c) | 17.8 Resource Management Patterns > 17.8.2 SPI Bus Broker Task | executed |
| [13-17-9-2-code-example-multi-phase-startup-with-event-group-sync.c](13-17-9-2-code-example-multi-phase-startup-with-event-group-sync.c) | 17.9 System Startup and Shutdown > 17.9.2 Code Example: Multi-Phase Startup with Event Group Sync | executed |
| [14-lab17_main.c](14-lab17_main.c) | 17.12 Lab 17: Designing an Event-Driven Application > Complete Code Listing | executed |
