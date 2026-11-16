# Book code on FreeRTOS / STM32F4 (QEMU) - execution results

Source: `RTOS_Book_KDP_Paperback_7x10.docx`. Every *Source Code* block was extracted in document order and, where it is C, wrapped into a runnable unit and executed on **FreeRTOS V10.6.1** (the version the book's Appendix F pins) using the **ARM_CM4F port** on **QEMU `netduinoplus2`** (STM32F405, Cortex-M4F, 168 MHz).

## 1. What was extracted

* **436** code blocks in total.
* c: 301, other: 55, diagram: 31, shell: 17, other-rtos: 13, linker: 6, config-other: 6, cmake: 5, asm: 2
* **301** are C listings (plus **13** assembled whole-section labs/programs run as additional units).

## 2. Outcome for the C listings

| Verdict | Count | Meaning |
|---|---:|---|
| EXECUTED | 151 | book code actually ran on FreeRTOS (program `main`, tasks/ISRs/init functions, or a statement fragment in a task) without fault |
| COMPILED_ONLY | 90 | types/macros/prototypes (or helpers that cannot be called generically): built, linked and booted, nothing to execute |
| EXECUTED_HELPERS | 45 | only helper functions; each was invoked with synthesized arguments in a separate run and returned/ran cleanly |
| HW_LIMITED | 4 | needs hardware QEMU does not model (CCM RAM, backup SRAM/PWR flags, ITM/SWO, STM32H7 memory) |
| BOOK_ISSUE | 3 | listing is inconsistent with another listing of the book |
| NEEDS_OTHER_PORT | 2 | needs the FreeRTOS-MPU port |
| EXPECTED_FAILURE | 2 | documented anti-pattern; the failure is the demonstration (kernel assert fires as the book warns) |
| EXCERPT | 2 | illustrative excerpt that is not compilable C as written |
| BARE_METAL_BY_DESIGN | 1 | intentional super-loop without scheduler |
| NO_SCHEDULER | 1 | bootloader-style `main` that never starts the scheduler; loop ran for the whole window |

**196 of 301 C listings were genuinely executed on the target** (151 as written + 45 via synthesized-argument calls). A further 90 are declarations/helpers that were compiled, linked and booted.

Helper functions with arguments were also exercised separately for listings that have tasks/programs ({'RAN_OK': 70, 'ASSERT': 2, 'FAULT': 3, 'LINK_FAIL': 1, 'HANG': 1}); failures there reflect synthetic arguments and are not counted against the book.

### Labs and multi-listing programs (whole section assembled)

| Unit | Section | Status | Note |
|---|---|---|---|
| S005 | Chapter 1: Rea: 1.3 Bare Metal vs RTOS vs Linux | SCHED_NOT_STARTED | bare-metal super-loop listing: it intentionally never starts an RTOS scheduler; the loop itself ran for the wh |
| S015 | Chapter 2: Dev: 2.4 The Boot Sequence Explained | RAN_OK |  |
| S036 | Chapter 3: Tas: 3.3 Creating Tasks | RAN_OK |  |
| S079 | Chapter 4: Que: 4.11 Lab 4: Multi-Channel Data Pipeline | RAN_OK |  |
| S155 | Chapter 7: Mem: 7.10 Lab 7: Memory Budget and Overflow Detection | RAN_OK |  |
| S208 | Chapter 9: Dev: 9.11 Lab 9: Building a Reusable UART Driver | RAN_OK |  |
| S234 | Chapter 10: Po: 10.10 Lab 10: Tickless Idle Power Optimization | RAN_OK |  |
| S265 | Chapter 12: De: 12.5 Percepio Tracealyzer Integration | RAN_OK |  |
| S278 | Chapter 12: De: 12.8 Post-Mortem Analysis | SCHED_NOT_STARTED | main() never reaches vTaskStartScheduler(): stuck in BackupSRAM_Enable (unit.c:70 (discriminator 1)) polling t |
| S282 | Chapter 12: De: 12.11 Lab 12: Building a Flight Recorder and Diagnostic | RAN_OK |  |
| S328 | Chapter 14: Te: 14.11 Lab 14: Building a Test Pipeline | RAN_OK |  |
| S356 | Chapter 15: Co: 15.16 Lab 15: Pitfall Detection Workshop | BUILD_FAIL | assembled section inherits a failure from one of its listings (see members) |
| S394 | Chapter 19: Mu: 19.3 FreeRTOS SMP | RAN_OK |  |

## 3. Listings that did not run cleanly (and why)

| Block | Section | Status | Verdict | Diagnosis |
|---:|---|---|---|---|
| 5 | Chapter 1: 1.3 Bare Metal vs RTOS vs Linux | SCHED_NOT_STARTED | BARE_METAL_BY_DESIGN | bare-metal super-loop listing: it intentionally never starts an RTOS scheduler; the loop itself ran for the whole window |
| 142 | Chapter 7: 7.2 FreeRTOS Heap Implementations | FAULT | HW_LIMITED | bus fault at CCM RAM (0x10000000..): QEMU's STM32F405 model has no CCM RAM; the listing hard-codes it |
| 150 | Chapter 7: 7.5 Memory Protection Unit (MPU) | BUILD_FAIL | NEEDS_OTHER_PORT | uses the FreeRTOS-MPU port API (xTaskCreateRestricted / TaskParameters_t); this harness builds the standard ARM_CM4F port |
| 305 | Chapter 13 13.11 Lab 13: Health Monitor with Crash Lo | SCHED_NOT_STARTED | HW_LIMITED | main() never reaches vTaskStartScheduler(): stuck in bkpsram_init (unit.c:55) polling the PWR backup-regulator-ready flag (PWR peripheral is not modelled by QEMU) |
| 342 | Chapter 15 15.6 Pitfall: ISR Calling Non-Thread-Safe  | ASSERT | EXPECTED_FAILURE | the section is a documented pitfall/anti-pattern; FreeRTOS' own configASSERT catches it exactly as the book warns |
| 357 | Chapter 15 15.16 Lab 15: Pitfall Detection Workshop | ASSERT | EXPECTED_FAILURE | the section is a documented pitfall/anti-pattern; FreeRTOS' own configASSERT catches it exactly as the book warns |
| 358 | Chapter 15 15.16 Lab 15: Pitfall Detection Workshop | BUILD_FAIL | EXCERPT | illustrative excerpt: elided arguments (`/* ... */`) leave an incomplete call, not compilable as written |
| 362 | Chapter 16 16.2 Secure Boot and Chain of Trust | SCHED_NOT_STARTED | NO_SCHEDULER | main() does not start the scheduler within the window (stuck in book_main (unit.c:105 (discriminator 3))); bootloader-style listing whose loop ran for the whole window |
| 363 | Chapter 16 16.3 Memory Protection Unit (MPU) for Secu | BUILD_FAIL | NEEDS_OTHER_PORT | uses the FreeRTOS-MPU port API (xTaskCreateRestricted / TaskParameters_t); this harness builds the standard ARM_CM4F port |
| 365 | Chapter 16 16.5 Secure Communication | BUILD_FAIL | BOOK_ISSUE | calls can_receive(&id, frame, 8, timeout) with 4 arguments, but the book's can_driver.h declares can_receive(can_msg_t *msg, uint32_t timeout_ms) |
| 393 | Chapter 18 18.11 Lab 18: Creating a Certification Evi | BUILD_FAIL | EXCERPT | illustrative excerpt: a bare `if (...)` condition with no statement |
| 399 | Chapter 19 19.4 AMP with FreeRTOS | BUILD_FAIL | BOOK_ISSUE | listing uses member `telemetry_ring`, but the book's own ipc_shared.h (Lab 19) defines `sensor_ring` |
| 400 | Chapter 19 19.4 AMP with FreeRTOS | BUILD_FAIL | BOOK_ISSUE | listing uses member `telemetry_ring`, but the book's own ipc_shared.h (Lab 19) defines `sensor_ring` |
| 408 | Chapter 19 19.10 Lab 19: AMP Inter-Core Communication | FAULT | HW_LIMITED | bus fault at 0x3004xxxx (STM32H7 D3 SRAM): H7-only memory, the listing targets the dual-core STM32H7 |
| 409 | Chapter 19 19.10 Lab 19: AMP Inter-Core Communication | FAULT | HW_LIMITED | bus fault at 0x3004xxxx (STM32H7 D3 SRAM): H7-only memory, the listing targets the dual-core STM32H7 |

## 4. Per-chapter coverage

| Chapter | Executed | Helpers | Compiled-only | Expected-fail | Other |
|---|---:|---:|---:|---:|---:|
| How to Use This Book | 1 | 0 | 0 | 0 | 0 |
| Chapter 1 | 2 | 0 | 1 | 0 | 1 |
| Chapter 2 | 3 | 0 | 12 | 0 | 0 |
| Chapter 3 | 4 | 0 | 4 | 0 | 0 |
| Chapter 4 | 20 | 0 | 9 | 0 | 0 |
| Chapter 5 | 12 | 1 | 9 | 0 | 0 |
| Chapter 6 | 4 | 7 | 8 | 0 | 0 |
| Chapter 7 | 4 | 1 | 8 | 0 | 2 |
| Chapter 8 | 10 | 1 | 4 | 0 | 0 |
| Chapter 9 | 7 | 10 | 7 | 0 | 0 |
| Chapter 10 | 12 | 1 | 6 | 0 | 0 |
| Chapter 11 | 14 | 1 | 1 | 0 | 0 |
| Chapter 12 | 11 | 2 | 9 | 0 | 0 |
| Chapter 13 | 4 | 7 | 2 | 0 | 1 |
| Chapter 14 | 4 | 6 | 4 | 0 | 0 |
| Chapter 15 | 24 | 0 | 0 | 2 | 1 |
| Chapter 16 | 2 | 1 | 0 | 0 | 3 |
| Chapter 17 | 7 | 4 | 3 | 0 | 0 |
| Chapter 18 | 1 | 0 | 0 | 0 | 1 |
| Chapter 19 | 5 | 3 | 1 | 0 | 4 |
| Appendix D | 0 | 0 | 1 | 0 | 0 |
| Appendix F | 0 | 0 | 1 | 0 | 0 |

## 5. Compiler warnings worth a look (excluding implicit declarations of helpers the book does not show)

* block 15: `unit.c:15:21: warning: initialization discards 'volatile' qualifier from pointer target type [-Wdiscarded-qualifiers]`
* block 15: `unit.c:16:21: warning: initialization discards 'volatile' qualifier from pointer target type [-Wdiscarded-qualifiers]`
* block 202: `unit.c:22:29: warning: passing argument 2 of 'xQueueGenericSend' discards 'volatile' qualifier from pointer target type [-Wdiscarded-qualifiers]`
* block 280: `unit.c:46:1: warning: control reaches end of non-void function [-Wreturn-type]`
* block 333: `unit.c:21:29: warning: format '%s' expects argument of type 'char *', but argument 2 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]`
* block 333: `unit.c:24:29: warning: format '%s' expects argument of type 'char *', but argument 2 has type 'uint32_t' {aka 'long unsigned int'} [-Wformat=]`
* block 408: `unit.c:42:27: warning: passing argument 1 of 'ipc_ring_push' discards 'volatile' qualifier from pointer target type [-Wdiscarded-qualifiers]`
* block 409: `unit.c:51:29: warning: passing argument 1 of 'ipc_ring_pop' discards 'volatile' qualifier from pointer target type [-Wdiscarded-qualifiers]`

## 6. Full per-block table

| Block | Kind | Chapter / section | Verdict | Status | How executed |
|---:|---|---|---|---|---|
| 1 | c | How to Use T  | EXECUTED | RAN_OK | runs: vExampleTask |
| 2 | diagram | Chapter 1: R 1.2 Core Timing Concepts | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 3 | other | Chapter 1: R 1.2 Core Timing Concepts | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 4 | other | Chapter 1: R 1.2 Core Timing Concepts | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 5 | c | Chapter 1: R 1.3 Bare Metal vs RTOS vs Linux | BARE_METAL_BY_DESIGN | SCHED_NOT_STARTED | program (main) |
| 6 | c | Chapter 1: R 1.3 Bare Metal vs RTOS vs Linux | EXECUTED | RAN_OK | runs: vControlTask,vCommsTask |
| 7 | other | Chapter 1: R 1.4 RTOS Kernel Architecture | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 8 | diagram | Chapter 1: R 1.4 RTOS Kernel Architecture | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 9 | diagram | Chapter 1: R 1.5 FreeRTOS Architecture Overview | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 10 | c | Chapter 1: R 1.5 FreeRTOS Architecture Overview | COMPILED_ONLY | RAN_OK | declarations only |
| 11 | c | Chapter 1: R 1.10 Lab 1: Baseline Timing Characte | EXECUTED | RAN_OK | program (main) |
| 12 | shell | Chapter 2: D 2.2 Toolchain Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 13 | other | Chapter 2: D 2.2 Toolchain Setup | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 14 | other | Chapter 2: D 2.3 Project Repository Structure | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 15 | c | Chapter 2: D 2.4 The Boot Sequence Explained | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 16 | c | Chapter 2: D 2.4 The Boot Sequence Explained | EXECUTED | RAN_OK | program (main) |
| 17 | diagram | Chapter 2: D 2.5 Clock Tree Configuration | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 18 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 19 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 20 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 21 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 22 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 23 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 24 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 25 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 26 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 27 | c | Chapter 2: D 2.7 FreeRTOSConfig.h Deep Dive | COMPILED_ONLY | RAN_OK | declarations only |
| 28 | c | Chapter 2: D 2.8 First Two Tasks | EXECUTED | RAN_OK | runs: vHeartbeatTask |
| 29 | c | Chapter 2: D 2.8 First Two Tasks | EXECUTED | RAN_OK | runs: vMonitorTask |
| 30 | c | Chapter 2: D 2.10 FreeRTOS Hook Functions | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 31 | other-rtos | Chapter 2: D 2.13 Zephyr Portability Note | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 32 | diagram | Chapter 3: T 3.1 The Task Abstraction | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 33 | diagram | Chapter 3: T 3.1 The Task Abstraction | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 34 | other | Chapter 3: T 3.1 The Task Abstraction | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 35 | diagram | Chapter 3: T 3.2 Task States and Transitions | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 36 | c | Chapter 3: T 3.3 Creating Tasks | COMPILED_ONLY | RAN_OK | declarations only |
| 37 | c | Chapter 3: T 3.3 Creating Tasks | COMPILED_ONLY | RAN_OK | declarations only |
| 38 | c | Chapter 3: T 3.3 Creating Tasks | EXECUTED | RAN_OK | program (main) |
| 39 | other | Chapter 3: T 3.4 Priority Assignment Methodology | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 40 | other | Chapter 3: T 3.6 Context Switch Internals | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 41 | c | Chapter 3: T 3.6 Context Switch Internals | EXECUTED | RAN_OK | fragment in runner task |
| 42 | c | Chapter 3: T 3.7 The Idle Task and Idle Hook | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 43 | c | Chapter 3: T 3.8 Runtime Statistics and Schedulab | COMPILED_ONLY | RAN_OK | declarations only |
| 44 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 45 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 46 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 47 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 48 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 49 | other | Chapter 3: T 3.8 Runtime Statistics and Schedulab | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 50 | c | Chapter 3: T 3.9 Starvation and Design Guidelines | EXECUTED | RAN_OK | runs: vTaskA |
| 51 | c | Chapter 3: T 3.12 Lab 3: Multi-Task Scheduling An | EXECUTED | RAN_OK | program (main) |
| 52 | c | Chapter 4: Q 4.2 Queue Fundamentals | COMPILED_ONLY | RAN_OK | declarations only |
| 53 | c | Chapter 4: Q 4.2 Queue Fundamentals | EXECUTED | RAN_OK | fragment in runner task |
| 54 | c | Chapter 4: Q 4.2 Queue Fundamentals | EXECUTED | RAN_OK | fragment in runner task |
| 55 | other | Chapter 4: Q 4.2 Queue Fundamentals | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 56 | other | Chapter 4: Q 4.2 Queue Fundamentals | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 57 | c | Chapter 4: Q 4.3 Sending and Receiving | COMPILED_ONLY | RAN_OK | declarations only |
| 58 | c | Chapter 4: Q 4.3 Sending and Receiving | EXECUTED | RAN_OK | fragment in runner task |
| 59 | c | Chapter 4: Q 4.3 Sending and Receiving | COMPILED_ONLY | RAN_OK | declarations only |
| 60 | c | Chapter 4: Q 4.3 Sending and Receiving | EXECUTED | RAN_OK | definitions + usage snippet; runs: vSensorTask,vProcessingTask |
| 61 | c | Chapter 4: Q 4.4 ISR-Safe Queue Operations | COMPILED_ONLY | RAN_OK | declarations only |
| 62 | c | Chapter 4: Q 4.4 ISR-Safe Queue Operations | COMPILED_ONLY | RAN_OK | declarations only |
| 63 | c | Chapter 4: Q 4.4 ISR-Safe Queue Operations | EXECUTED | RAN_OK | fragment in runner task |
| 64 | c | Chapter 4: Q 4.4 ISR-Safe Queue Operations | EXECUTED | RAN_OK | runs: vUartProcessTask,USART2_IRQn |
| 65 | c | Chapter 4: Q 4.5 Queue Sets | EXECUTED | RAN_OK | fragment in runner task |
| 66 | c | Chapter 4: Q 4.5 Queue Sets | EXECUTED | RAN_OK | fragment in runner task |
| 67 | c | Chapter 4: Q 4.6 Message Buffers and Stream Buffe | EXECUTED | RAN_OK | fragment in runner task |
| 68 | c | Chapter 4: Q 4.6 Message Buffers and Stream Buffe | EXECUTED | RAN_OK | fragment in runner task |
| 69 | c | Chapter 4: Q 4.6 Message Buffers and Stream Buffe | EXECUTED | RAN_OK | fragment in runner task |
| 70 | c | Chapter 4: Q 4.6 Message Buffers and Stream Buffe | EXECUTED | RAN_OK | fragment in runner task |
| 71 | c | Chapter 4: Q 4.6 Message Buffers and Stream Buffe | EXECUTED | RAN_OK | runs: vLogTask |
| 72 | c | Chapter 4: Q 4.7 Task Notifications as Lightweigh | EXECUTED | RAN_OK | runs: vProcessingTask,EXTI0_IRQn |
| 73 | c | Chapter 4: Q 4.7 Task Notifications as Lightweigh | COMPILED_ONLY | RAN_OK | declarations only |
| 74 | c | Chapter 4: Q 4.7 Task Notifications as Lightweigh | COMPILED_ONLY | RAN_OK | declarations only |
| 75 | c | Chapter 4: Q 4.8 Design Patterns and Best Practic | COMPILED_ONLY | RAN_OK | declarations only |
| 76 | c | Chapter 4: Q 4.8 Design Patterns and Best Practic | EXECUTED | RAN_OK | fragment in runner task |
| 77 | c | Chapter 4: Q 4.8 Design Patterns and Best Practic | EXECUTED | RAN_OK | fragment in runner task |
| 78 | diagram | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 79 | c | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | COMPILED_ONLY | RAN_OK | declarations only |
| 80 | c | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | EXECUTED | RAN_OK | runs: vSensorTask |
| 81 | c | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | EXECUTED | RAN_OK | runs: TIM2_IRQn |
| 82 | c | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | EXECUTED | RAN_OK | runs: vAggregatorTask |
| 83 | c | Chapter 4: Q 4.11 Lab 4: Multi-Channel Data Pipel | EXECUTED | RAN_OK | program (main) |
| 84 | c | Chapter 5: S 5.1 The Critical Section Problem | EXECUTED | RAN_OK | fragment in runner task |
| 85 | asm | Chapter 5: S 5.1 The Critical Section Problem | NOT_CODE | NOT_EXECUTABLE | assembly illustration - not a C listing |
| 86 | c | Chapter 5: S 5.1 The Critical Section Problem | EXECUTED | RAN_OK | fragment in runner task |
| 87 | c | Chapter 5: S 5.2 Binary Semaphores | COMPILED_ONLY | RAN_OK | declarations only |
| 88 | c | Chapter 5: S 5.2 Binary Semaphores | COMPILED_ONLY | RAN_OK | declarations only |
| 89 | c | Chapter 5: S 5.2 Binary Semaphores | COMPILED_ONLY | RAN_OK | declarations only |
| 90 | c | Chapter 5: S 5.2 Binary Semaphores | EXECUTED | RAN_OK | runs: vAdcProcessingTask,vSetupAdc,ADC_IRQn |
| 91 | c | Chapter 5: S 5.3 Counting Semaphores | COMPILED_ONLY | RAN_OK | declarations only |
| 92 | c | Chapter 5: S 5.3 Counting Semaphores | EXECUTED | RAN_OK | runs: vInitDmaPool |
| 93 | c | Chapter 5: S 5.4 Mutexes and Priority Inheritance | COMPILED_ONLY | RAN_OK | declarations only |
| 94 | other | Chapter 5: S 5.4 Mutexes and Priority Inheritance | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 95 | c | Chapter 5: S 5.4 Mutexes and Priority Inheritance | COMPILED_ONLY | RAN_OK | declarations only |
| 96 | c | Chapter 5: S 5.5 Critical Sections and Interrupt  | EXECUTED | RAN_OK | fragment in runner task |
| 97 | c | Chapter 5: S 5.5 Critical Sections and Interrupt  | EXECUTED | RAN_OK | fragment in runner task |
| 98 | c | Chapter 5: S 5.5 Critical Sections and Interrupt  | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 99 | c | Chapter 5: S 5.5 Critical Sections and Interrupt  | EXECUTED | RAN_OK | fragment in runner task |
| 100 | diagram | Chapter 5: S 5.6 Deadlock | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 101 | diagram | Chapter 5: S 5.6 Deadlock | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 102 | c | Chapter 5: S 5.6 Deadlock | EXECUTED | RAN_OK | runs: vTaskSensor,vTaskDiagnostic |
| 103 | c | Chapter 5: S 5.6 Deadlock | EXECUTED | RAN_OK | runs: vTaskSensor,vTaskDiagnostic |
| 104 | c | Chapter 5: S 5.6 Deadlock | EXECUTED | RAN_OK | fragment in runner task |
| 105 | c | Chapter 5: S 5.7 Event Groups | COMPILED_ONLY | RAN_OK | declarations only |
| 106 | c | Chapter 5: S 5.7 Event Groups | COMPILED_ONLY | RAN_OK | declarations only |
| 107 | c | Chapter 5: S 5.7 Event Groups | COMPILED_ONLY | RAN_OK | declarations only |
| 108 | c | Chapter 5: S 5.7 Event Groups | EXECUTED | RAN_OK | program (main) |
| 109 | diagram | Chapter 5: S 5.8 Design Patterns | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 110 | other | Chapter 5: S 5.8 Design Patterns | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 111 | other | Chapter 5: S 5.8 Design Patterns | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 112 | c | Chapter 5: S 5.11 Lab 5: Priority Inversion Demon | EXECUTED | RAN_OK | program (main) |
| 113 | c | Chapter 6: S 6.1 The Need for Timed Actions | EXECUTED | RAN_OK | runs: vWatchdogFeedTask |
| 114 | diagram | Chapter 6: S 6.2 FreeRTOS Software Timer Architec | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 115 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 116 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 117 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 118 | other | Chapter 6: S 6.3 Creating and Managing Timers | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 119 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 120 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 121 | c | Chapter 6: S 6.3 Creating and Managing Timers | COMPILED_ONLY | RAN_OK | declarations only |
| 122 | c | Chapter 6: S 6.3 Creating and Managing Timers | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 123 | c | Chapter 6: S 6.4 Timer Callback Rules and Restric | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 124 | c | Chapter 6: S 6.5 Timing Accuracy and Jitter | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 125 | c | Chapter 6: S 6.5 Timing Accuracy and Jitter | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 126 | c | Chapter 6: S 6.6 Deferred Interrupt Processing Pa | COMPILED_ONLY | RAN_OK | declarations only |
| 127 | c | Chapter 6: S 6.6 Deferred Interrupt Processing Pa | COMPILED_ONLY | RAN_OK | declarations only |
| 128 | other | Chapter 6: S 6.6 Deferred Interrupt Processing Pa | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 129 | other | Chapter 6: S 6.6 Deferred Interrupt Processing Pa | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 130 | c | Chapter 6: S 6.6 Deferred Interrupt Processing Pa | EXECUTED | RAN_OK | runs: EXTI0_IRQn |
| 131 | c | Chapter 6: S 6.7 Advanced Patterns | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 132 | c | Chapter 6: S 6.7 Advanced Patterns | EXECUTED | RAN_OK | runs: vStartCommunication |
| 133 | c | Chapter 6: S 6.7 Advanced Patterns | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 134 | c | Chapter 6: S 6.7 Advanced Patterns | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 135 | other-rtos | Chapter 6: S 6.8 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 136 | other-rtos | Chapter 6: S 6.8 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 137 | other-rtos | Chapter 6: S 6.8 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 138 | other-rtos | Chapter 6: S 6.8 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 139 | c | Chapter 6: S 6.10 Lab 6: Software Timer Precision | EXECUTED | RAN_OK | program (main) |
| 140 | diagram | Chapter 7: M 7.1 Memory Landscape of a Cortex-M S | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 141 | c | Chapter 7: M 7.2 FreeRTOS Heap Implementations | COMPILED_ONLY | RAN_OK | declarations only |
| 142 | c | Chapter 7: M 7.2 FreeRTOS Heap Implementations | HW_LIMITED | FAULT | fragment in runner task |
| 143 | c | Chapter 7: M 7.2 FreeRTOS Heap Implementations | COMPILED_ONLY | RAN_OK | declarations only |
| 144 | linker | Chapter 7: M 7.2 FreeRTOS Heap Implementations | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 145 | c | Chapter 7: M 7.3 Static vs Dynamic Allocation | EXECUTED | RAN_OK | fragment in runner task |
| 146 | c | Chapter 7: M 7.3 Static vs Dynamic Allocation | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 147 | c | Chapter 7: M 7.3 Static vs Dynamic Allocation | COMPILED_ONLY | RAN_OK | declarations only |
| 148 | c | Chapter 7: M 7.4 Stack Sizing and Overflow Detect | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 149 | c | Chapter 7: M 7.4 Stack Sizing and Overflow Detect | EXECUTED | RAN_OK | runs: vStackMonitorTask |
| 150 | c | Chapter 7: M 7.5 Memory Protection Unit (MPU) | NEEDS_OTHER_PORT | BUILD_FAIL | does not compile even with stubs/context |
| 151 | c | Chapter 7: M 7.6 Fragmentation Analysis and Preve | EXECUTED | RAN_OK | fragment in runner task |
| 152 | c | Chapter 7: M 7.6 Fragmentation Analysis and Preve | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 153 | c | Chapter 7: M 7.7 Memory Debugging Techniques | COMPILED_ONLY | RAN_OK | declarations only |
| 154 | c | Chapter 7: M 7.7 Memory Debugging Techniques | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 155 | c | Chapter 7: M 7.10 Lab 7: Memory Budget and Overfl | COMPILED_ONLY | RAN_OK | declarations only |
| 156 | c | Chapter 7: M 7.10 Lab 7: Memory Budget and Overfl | EXECUTED | RAN_OK | program (main) |
| 157 | other | Chapter 8: I 8.1 Interrupts in Real-Time Context | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 158 | other | Chapter 8: I 8.2 ARM Cortex-M Interrupt Architect | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 159 | diagram | Chapter 8: I 8.2 ARM Cortex-M Interrupt Architect | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 160 | c | Chapter 8: I 8.3 FreeRTOS Interrupt Priority Mode | COMPILED_ONLY | RAN_OK | declarations only |
| 161 | c | Chapter 8: I 8.3 FreeRTOS Interrupt Priority Mode | COMPILED_ONLY | RAN_OK | declarations only |
| 162 | other | Chapter 8: I 8.3 FreeRTOS Interrupt Priority Mode | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 163 | c | Chapter 8: I 8.3 FreeRTOS Interrupt Priority Mode | COMPILED_ONLY | RAN_OK | declarations only |
| 164 | c | Chapter 8: I 8.4 ISR Design Patterns | EXECUTED | RAN_OK | runs: DMA1_Stream6_IRQn |
| 165 | c | Chapter 8: I 8.4 ISR Design Patterns | EXECUTED | RAN_OK | runs: ADC_IRQn |
| 166 | c | Chapter 8: I 8.4 ISR Design Patterns | EXECUTED | RAN_OK | runs: EXTI0_IRQn |
| 167 | c | Chapter 8: I 8.5 The FromISR API Family | EXECUTED | RAN_OK | fragment in runner task |
| 168 | c | Chapter 8: I 8.5 The FromISR API Family | EXECUTED | RAN_OK | runs: USART2_IRQn |
| 169 | c | Chapter 8: I 8.6 Interrupt Nesting | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 170 | diagram | Chapter 8: I 8.6 Interrupt Nesting | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 171 | other | Chapter 8: I 8.6 Interrupt Nesting | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 172 | linker | Chapter 8: I 8.6 Interrupt Nesting | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 173 | asm | Chapter 8: I 8.6 Interrupt Nesting | NOT_CODE | NOT_EXECUTABLE | assembly illustration - not a C listing |
| 174 | c | Chapter 8: I 8.7 Direct Memory Access (DMA) Integ | EXECUTED | RAN_OK | runs: UART_DMA_Init,USART2_IRQn |
| 175 | c | Chapter 8: I 8.8 Measuring and Reducing Interrupt | EXECUTED | RAN_OK | runs: EXTI1_IRQn |
| 176 | c | Chapter 8: I 8.8 Measuring and Reducing Interrupt | EXECUTED | RAN_OK | fragment in runner task |
| 177 | c | Chapter 8: I 8.8 Measuring and Reducing Interrupt | EXECUTED | RAN_OK | runs: TIM2_IRQn |
| 178 | c | Chapter 8: I 8.10 Common Pitfalls | COMPILED_ONLY | RAN_OK | declarations only |
| 179 | c | Chapter 8: I 8.11 Lab 8: Interrupt Latency Measur | EXECUTED | RAN_OK | program (main) |
| 180 | c | Chapter 9: D 9.2 Four-Layer Driver Model | COMPILED_ONLY | RAN_OK | declarations only |
| 181 | c | Chapter 9: D 9.2 Four-Layer Driver Model | EXECUTED | RAN_OK | fragment in runner task |
| 182 | c | Chapter 9: D 9.2 Four-Layer Driver Model | COMPILED_ONLY | RAN_OK | declarations only |
| 183 | diagram | Chapter 9: D 9.2 Four-Layer Driver Model | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 184 | c | Chapter 9: D 9.3 UART Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 185 | c | Chapter 9: D 9.3 UART Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 186 | c | Chapter 9: D 9.3 UART Driver Case Study | EXECUTED | RAN_OK | runs: USART2_IRQn |
| 187 | c | Chapter 9: D 9.3 UART Driver Case Study | COMPILED_ONLY | RAN_OK | declarations only |
| 188 | c | Chapter 9: D 9.3 UART Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 189 | c | Chapter 9: D 9.4 SPI Driver Case Study | COMPILED_ONLY | RAN_OK | declarations only |
| 190 | c | Chapter 9: D 9.4 SPI Driver Case Study | EXECUTED | RAN_OK | runs: spi_init |
| 191 | c | Chapter 9: D 9.4 SPI Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 192 | c | Chapter 9: D 9.5 I2C Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 193 | c | Chapter 9: D 9.5 I2C Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 194 | c | Chapter 9: D 9.5 I2C Driver Case Study | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 195 | c | Chapter 9: D 9.6 CAN Bus Driver | COMPILED_ONLY | RAN_OK | declarations only |
| 196 | c | Chapter 9: D 9.6 CAN Bus Driver | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 197 | other | Chapter 9: D 9.7 The Fake HAL Pattern for Testing | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 198 | c | Chapter 9: D 9.7 The Fake HAL Pattern for Testing | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 199 | c | Chapter 9: D 9.7 The Fake HAL Pattern for Testing | EXECUTED | RAN_OK | runs: setUp |
| 200 | c | Chapter 9: D 9.8 Driver Design Patterns | COMPILED_ONLY | RAN_OK | declarations only |
| 201 | c | Chapter 9: D 9.8 Driver Design Patterns | EXECUTED | RAN_OK | fragment in runner task |
| 202 | c | Chapter 9: D 9.8 Driver Design Patterns | EXECUTED | RAN_OK | fragment in runner task |
| 203 | c | Chapter 9: D 9.8 Driver Design Patterns | COMPILED_ONLY | RAN_OK | declarations only |
| 204 | other-rtos | Chapter 9: D 9.9 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 205 | other-rtos | Chapter 9: D 9.9 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 206 | other-rtos | Chapter 9: D 9.9 QNX and Zephyr Portability Notes | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 207 | other | Chapter 9: D 9.11 Lab 9: Building a Reusable UART | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 208 | c | Chapter 9: D 9.11 Lab 9: Building a Reusable UART | EXECUTED | RAN_OK | program (main) |
| 209 | c | Chapter 9: D 9.11 Lab 9: Building a Reusable UART | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 210 | shell | Chapter 9: D 9.11 Lab 9: Building a Reusable UART | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 211 | other | Chapter 10:  10.1 Power Constraints in Embedded S | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 212 | other | Chapter 10:  10.1 Power Constraints in Embedded S | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 213 | c | Chapter 10:  10.2 STM32 Low-Power Modes | EXECUTED | RAN_OK | fragment in runner task |
| 214 | c | Chapter 10:  10.2 STM32 Low-Power Modes | EXECUTED | RAN_OK | fragment in runner task |
| 215 | c | Chapter 10:  10.2 STM32 Low-Power Modes | EXECUTED | RAN_OK | fragment in runner task |
| 216 | other | Chapter 10:  10.3 FreeRTOS Tickless Idle | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 217 | other | Chapter 10:  10.3 FreeRTOS Tickless Idle | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 218 | c | Chapter 10:  10.3 FreeRTOS Tickless Idle | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 219 | c | Chapter 10:  10.4 Custom Tickless Implementation  | COMPILED_ONLY | RAN_OK | declarations only |
| 220 | c | Chapter 10:  10.4 Custom Tickless Implementation  | COMPILED_ONLY | RAN_OK | declarations only |
| 221 | c | Chapter 10:  10.4 Custom Tickless Implementation  | EXECUTED | RAN_OK | runs: RTC_WakeupTimer_Init |
| 222 | c | Chapter 10:  10.4 Custom Tickless Implementation  | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 223 | c | Chapter 10:  10.4 Custom Tickless Implementation  | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 224 | c | Chapter 10:  10.4 Custom Tickless Implementation  | EXECUTED | RAN_OK | runs: RTC_WKUP_IRQn |
| 225 | c | Chapter 10:  10.5 Peripheral Power Management | EXECUTED | RAN_OK | fragment in runner task |
| 226 | c | Chapter 10:  10.5 Peripheral Power Management | EXECUTED | RAN_OK | fragment in runner task |
| 227 | other | Chapter 10:  10.6 Power Profiling and Measurement | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 228 | other | Chapter 10:  10.6 Power Profiling and Measurement | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 229 | other | Chapter 10:  10.6 Power Profiling and Measurement | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 230 | c | Chapter 10:  10.7 Design Patterns for Low Power | EXECUTED | RAN_OK | runs: vBadTask,vGoodTask |
| 231 | c | Chapter 10:  10.7 Design Patterns for Low Power | EXECUTED | RAN_OK | runs: vSensorTask |
| 232 | c | Chapter 10:  10.9 Common Pitfalls | EXECUTED | RAN_OK | fragment in runner task |
| 233 | other | Chapter 10:  10.10 Lab 10: Tickless Idle Power Op | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 234 | c | Chapter 10:  10.10 Lab 10: Tickless Idle Power Op | EXECUTED | RAN_OK | program (main) |
| 235 | c | Chapter 10:  10.10 Lab 10: Tickless Idle Power Op | COMPILED_ONLY | RAN_OK | declarations only |
| 236 | c | Chapter 10:  10.10 Lab 10: Tickless Idle Power Op | COMPILED_ONLY | RAN_OK | declarations only |
| 237 | c | Chapter 10:  10.10 Lab 10: Tickless Idle Power Op | EXECUTED | RAN_OK | fragment in runner task |
| 238 | other | Chapter 11:  11.2 STM32F407 Clock Architecture | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 239 | diagram | Chapter 11:  11.2 STM32F407 Clock Architecture | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 240 | c | Chapter 11:  11.2 STM32F407 Clock Architecture | EXECUTED | RAN_OK | fragment in runner task |
| 241 | c | Chapter 11:  11.2 STM32F407 Clock Architecture | EXECUTED | RAN_OK | runs: NMI |
| 242 | c | Chapter 11:  11.3 SysTick Timer and RTOS Tick | EXECUTED | RAN_OK | fragment in runner task |
| 243 | c | Chapter 11:  11.3 SysTick Timer and RTOS Tick | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 244 | other | Chapter 11:  11.4 Hardware Timers for Precise Tim | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 245 | c | Chapter 11:  11.4 Hardware Timers for Precise Tim | EXECUTED | RAN_OK | runs: TIM2_freq_init,TIM2_IRQn |
| 246 | c | Chapter 11:  11.4 Hardware Timers for Precise Tim | EXECUTED | RAN_OK | fragment in runner task |
| 247 | c | Chapter 11:  11.5 DWT Cycle Counter | EXECUTED | RAN_OK | fragment in runner task |
| 248 | other | Chapter 11:  11.5 DWT Cycle Counter | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 249 | other | Chapter 11:  11.5 DWT Cycle Counter | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 250 | c | Chapter 11:  11.5 DWT Cycle Counter | EXECUTED | RAN_OK | fragment in runner task |
| 251 | c | Chapter 11:  11.5 DWT Cycle Counter | COMPILED_ONLY | RAN_OK | declarations only |
| 252 | c | Chapter 11:  11.5 DWT Cycle Counter | EXECUTED | RAN_OK | usage snippet in runner task |
| 253 | c | Chapter 11:  11.7 Temporal Determinism Strategies | EXECUTED | RAN_OK | fragment in runner task |
| 254 | c | Chapter 11:  11.7 Temporal Determinism Strategies | EXECUTED | RAN_OK | fragment in runner task |
| 255 | c | Chapter 11:  11.7 Temporal Determinism Strategies | EXECUTED | RAN_OK | fragment in runner task |
| 256 | c | Chapter 11:  11.7 Temporal Determinism Strategies | EXECUTED | RAN_OK | runs: vPeriodicTask,vPeriodicTask__v2 |
| 257 | c | Chapter 11:  11.8 Real-Time Clock (RTC) for Calen | EXECUTED | RAN_OK | runs: RTC_Init |
| 258 | c | Chapter 11:  11.11 Lab 11: Timing Accuracy and WC | EXECUTED | RAN_OK | program (main) |
| 259 | c | Chapter 12:  12.2 printf-Style Debugging | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 260 | c | Chapter 12:  12.2 printf-Style Debugging | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 261 | c | Chapter 12:  12.4 FreeRTOS Trace Hooks | COMPILED_ONLY | RAN_OK | declarations only |
| 262 | c | Chapter 12:  12.4 FreeRTOS Trace Hooks | EXECUTED | RAN_OK | runs: FR_Init |
| 263 | c | Chapter 12:  12.4 FreeRTOS Trace Hooks | COMPILED_ONLY | RAN_OK | declarations only |
| 264 | c | Chapter 12:  12.4 FreeRTOS Trace Hooks | COMPILED_ONLY | RAN_OK | declarations only |
| 265 | c | Chapter 12:  12.5 Percepio Tracealyzer Integratio | COMPILED_ONLY | RAN_OK | declarations only |
| 266 | c | Chapter 12:  12.5 Percepio Tracealyzer Integratio | COMPILED_ONLY | RAN_OK | declarations only |
| 267 | c | Chapter 12:  12.5 Percepio Tracealyzer Integratio | EXECUTED | RAN_OK | program (main) |
| 268 | c | Chapter 12:  12.6 SEGGER SystemView Integration | COMPILED_ONLY | RAN_OK | declarations only |
| 269 | c | Chapter 12:  12.6 SEGGER SystemView Integration | EXECUTED | RAN_OK | fragment in runner task |
| 270 | c | Chapter 12:  12.6 SEGGER SystemView Integration | EXECUTED | RAN_OK | runs: ADC_IRQn |
| 271 | other | Chapter 12:  12.7 Runtime Diagnostics APIs | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 272 | other | Chapter 12:  12.7 Runtime Diagnostics APIs | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 273 | c | Chapter 12:  12.7 Runtime Diagnostics APIs | COMPILED_ONLY | RAN_OK | declarations only |
| 274 | c | Chapter 12:  12.7 Runtime Diagnostics APIs | EXECUTED | RAN_OK | fragment in runner task |
| 275 | c | Chapter 12:  12.7 Runtime Diagnostics APIs | EXECUTED | RAN_OK | fragment in runner task |
| 276 | c | Chapter 12:  12.7 Runtime Diagnostics APIs | EXECUTED | RAN_OK | runs: vDiagnosticTask |
| 277 | other | Chapter 12:  12.8 Post-Mortem Analysis | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 278 | c | Chapter 12:  12.8 Post-Mortem Analysis | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 279 | c | Chapter 12:  12.8 Post-Mortem Analysis | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 280 | c | Chapter 12:  12.8 Post-Mortem Analysis | EXECUTED | RAN_OK | program (main) |
| 281 | linker | Chapter 12:  12.11 Lab 12: Building a Flight Reco | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 282 | c | Chapter 12:  12.11 Lab 12: Building a Flight Reco | EXECUTED | RAN_OK | runs: USART2_IRQn |
| 283 | c | Chapter 12:  12.11 Lab 12: Building a Flight Reco | EXECUTED | RAN_OK | runs: vConsoleTask |
| 284 | c | Chapter 12:  12.11 Lab 12: Building a Flight Reco | EXECUTED | RAN_OK | fragment in runner task |
| 285 | linker | Chapter 12:  12.11 Lab 12: Building a Flight Reco | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 286 | linker | Chapter 12:  12.11 Lab 12: Building a Flight Reco | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 287 | c | Chapter 13:  13.2 ARM Cortex-M Fault Handlers | EXECUTED | RAN_OK | fragment in runner task |
| 288 | other | Chapter 13:  13.2 ARM Cortex-M Fault Handlers | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 289 | c | Chapter 13:  13.2 ARM Cortex-M Fault Handlers | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 290 | other | Chapter 13:  13.3 Watchdog Timers | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 291 | c | Chapter 13:  13.3 Watchdog Timers | EXECUTED | RAN_OK | runs: iwdg_init |
| 292 | other | Chapter 13:  13.3 Watchdog Timers | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 293 | c | Chapter 13:  13.3 Watchdog Timers | EXECUTED | RAN_OK | runs: wwdg_init,WWDG_IRQn |
| 294 | diagram | Chapter 13:  13.4 Task-Level Health Monitoring | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 295 | c | Chapter 13:  13.4 Task-Level Health Monitoring | EXECUTED | RAN_OK | runs: monitor_task |
| 296 | c | Chapter 13:  13.5 Stack Overflow Detection and Re | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 297 | c | Chapter 13:  13.6 Assertion Framework | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 298 | c | Chapter 13:  13.7 Safe-State Design | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 299 | linker | Chapter 13:  13.7 Safe-State Design | NOT_CODE | NOT_EXECUTABLE | linker-script fragment - build input, not FreeRTOS code |
| 300 | c | Chapter 13:  13.8 Recovery Strategies | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 301 | c | Chapter 13:  13.8 Recovery Strategies | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 302 | c | Chapter 13:  13.8 Recovery Strategies | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 303 | c | Chapter 13:  13.8 Recovery Strategies | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 304 | c | Chapter 13:  13.9 QNX and Zephyr: Fault Managemen | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 305 | c | Chapter 13:  13.11 Lab 13: Health Monitor with Cr | HW_LIMITED | SCHED_NOT_STARTED | program (main) |
| 306 | other | Chapter 14:  14.1 The Testing Challenge for RTOS  | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 307 | cmake | Chapter 14:  14.2 Unit Testing on Host | NOT_CODE | NOT_EXECUTABLE | CMake build script - build system, not FreeRTOS code |
| 308 | shell | Chapter 14:  14.2 Unit Testing on Host | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 309 | shell | Chapter 14:  14.2 Unit Testing on Host | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 310 | c | Chapter 14:  14.2 Unit Testing on Host | COMPILED_ONLY | RAN_OK | declarations only |
| 311 | c | Chapter 14:  14.2 Unit Testing on Host | EXECUTED | RAN_OK | program (main) |
| 312 | c | Chapter 14:  14.2 Unit Testing on Host | EXECUTED | RAN_OK | program (main) |
| 313 | other | Chapter 14:  14.3 The Fake HAL Pattern (Expanded) | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 314 | c | Chapter 14:  14.3 The Fake HAL Pattern (Expanded) | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 315 | c | Chapter 14:  14.3 The Fake HAL Pattern (Expanded) | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 316 | c | Chapter 14:  14.3 The Fake HAL Pattern (Expanded) | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 317 | c | Chapter 14:  14.3 The Fake HAL Pattern (Expanded) | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 318 | c | Chapter 14:  14.4 Integration Testing | EXECUTED | RAN_OK | program (main) |
| 319 | shell | Chapter 14:  14.5 System Testing on Target | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 320 | c | Chapter 14:  14.5 System Testing on Target | COMPILED_ONLY | RAN_OK | declarations only |
| 321 | c | Chapter 14:  14.5 System Testing on Target | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 322 | diagram | Chapter 14:  14.6 Hardware-in-the-Loop (HIL) Test | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 323 | cmake | Chapter 14:  14.6 Hardware-in-the-Loop (HIL) Test | NOT_CODE | NOT_EXECUTABLE | CMake build script - build system, not FreeRTOS code |
| 324 | shell | Chapter 14:  14.7 Code Coverage for Embedded | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 325 | other | Chapter 14:  14.8 Continuous Integration for Embe | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 326 | config-other | Chapter 14:  14.8 Continuous Integration for Embe | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 327 | config-other | Chapter 14:  14.8 Continuous Integration for Embe | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 328 | c | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | COMPILED_ONLY | RAN_OK | declarations only |
| 329 | c | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | EXECUTED | RAN_OK | program (main) |
| 330 | c | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 331 | cmake | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | NOT_CODE | NOT_EXECUTABLE | CMake build script - build system, not FreeRTOS code |
| 332 | other | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 333 | c | Chapter 14:  14.11 Lab 14: Building a Test Pipeli | COMPILED_ONLY | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 334 | c | Chapter 15:  15.2 Pitfall: Unbounded Priority Inv | EXECUTED | RAN_OK | runs: vSensorTask,vProcessingTask,vControlTask |
| 335 | c | Chapter 15:  15.2 Pitfall: Unbounded Priority Inv | EXECUTED | RAN_OK | runs: vSensorTask,vProcessingTask,vControlTask |
| 336 | c | Chapter 15:  15.3 Pitfall: Deadlock | EXECUTED | RAN_OK | runs: vTaskA,vTaskB |
| 337 | c | Chapter 15:  15.3 Pitfall: Deadlock | EXECUTED | RAN_OK | runs: vTaskA,vTaskB |
| 338 | c | Chapter 15:  15.4 Pitfall: Stack Overflow | EXECUTED | RAN_OK | usage snippet in runner task |
| 339 | c | Chapter 15:  15.4 Pitfall: Stack Overflow | EXECUTED | RAN_OK | usage snippet in runner task |
| 340 | c | Chapter 15:  15.5 Pitfall: Race Conditions on Sha | EXECUTED | RAN_OK | runs: vProducerTask,vConsumerTask |
| 341 | c | Chapter 15:  15.5 Pitfall: Race Conditions on Sha | EXECUTED | RAN_OK | runs: vProducerTask,vConsumerTask |
| 342 | c | Chapter 15:  15.6 Pitfall: ISR Calling Non-Thread | EXPECTED_FAILURE | ASSERT | runs: EXTI0_IRQn |
| 343 | c | Chapter 15:  15.6 Pitfall: ISR Calling Non-Thread | EXECUTED | RAN_OK | runs: vDataHandlerTask,EXTI0_IRQn |
| 344 | c | Chapter 15:  15.7 Pitfall: Polling Instead of Blo | EXECUTED | RAN_OK | runs: vDataTask,EXTI0_IRQn |
| 345 | c | Chapter 15:  15.7 Pitfall: Polling Instead of Blo | EXECUTED | RAN_OK | runs: vDataTask,EXTI0_IRQn |
| 346 | c | Chapter 15:  15.8 Pitfall: Misusing vTaskDelay fo | EXECUTED | RAN_OK | runs: vSamplingTask |
| 347 | c | Chapter 15:  15.8 Pitfall: Misusing vTaskDelay fo | EXECUTED | RAN_OK | runs: vSamplingTask |
| 348 | c | Chapter 15:  15.9 Pitfall: Unbounded Priority Lev | EXECUTED | RAN_OK | fragment in runner task |
| 349 | c | Chapter 15:  15.10 Pitfall: Memory Leaks and Heap | EXECUTED | RAN_OK | runs: vPeriodicReportTask |
| 350 | c | Chapter 15:  15.10 Pitfall: Memory Leaks and Heap | EXECUTED | RAN_OK | runs: vPeriodicReportTask |
| 351 | c | Chapter 15:  15.11 Pitfall: Starving the Idle Tas | EXECUTED | RAN_OK | runs: vBadCpuBoundTask,vGoodCpuBoundTask |
| 352 | c | Chapter 15:  15.12 Pitfall: Interrupt Priority Mi | EXECUTED | RAN_OK | runs: vSetupPeripherals,EXTI0_IRQn |
| 353 | c | Chapter 15:  15.12 Pitfall: Interrupt Priority Mi | EXECUTED | RAN_OK | runs: vSetupPeripherals,EXTI0_IRQn,TIM1_UP_TIM10_IRQn |
| 354 | c | Chapter 15:  15.13 Pitfall: Global State and Hidd | EXECUTED | RAN_OK | runs: vSensorTask,vDisplayTask |
| 355 | c | Chapter 15:  15.13 Pitfall: Global State and Hidd | EXECUTED | RAN_OK | runs: vSensorTask,vDisplayTask |
| 356 | c | Chapter 15:  15.16 Lab 15: Pitfall Detection Work | EXECUTED | RAN_OK | runs: vControlTask |
| 357 | c | Chapter 15:  15.16 Lab 15: Pitfall Detection Work | EXPECTED_FAILURE | ASSERT | runs: DMA1_Stream6_IRQn |
| 358 | c | Chapter 15:  15.16 Lab 15: Pitfall Detection Work | EXCERPT | BUILD_FAIL | does not compile even with stubs/context |
| 359 | c | Chapter 15:  15.16 Lab 15: Pitfall Detection Work | EXECUTED | RAN_OK | runs: vCommTask,vLogTask |
| 360 | c | Chapter 15:  15.16 Lab 15: Pitfall Detection Work | EXECUTED | RAN_OK | runs: vAlertTask |
| 361 | diagram | Chapter 16:  16.2 Secure Boot and Chain of Trust | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 362 | c | Chapter 16:  16.2 Secure Boot and Chain of Trust | NO_SCHEDULER | SCHED_NOT_STARTED | program (main) |
| 363 | c | Chapter 16:  16.3 Memory Protection Unit (MPU) fo | NEEDS_OTHER_PORT | BUILD_FAIL | does not compile even with stubs/context |
| 364 | c | Chapter 16:  16.4 Cryptographic Operations | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 365 | c | Chapter 16:  16.5 Secure Communication | BOOK_ISSUE | BUILD_FAIL | does not compile even with stubs/context |
| 366 | diagram | Chapter 16:  16.6 Over-the-Air (OTA) Firmware Upd | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 367 | c | Chapter 16:  16.6 Over-the-Air (OTA) Firmware Upd | EXECUTED | RAN_OK | runs: ota_init |
| 368 | diagram | Chapter 16:  16.10 Lab 16: Secure Boot and Authen | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 369 | c | Chapter 16:  16.10 Lab 16: Secure Boot and Authen | EXECUTED | RAN_OK | runs: vCommandTask |
| 370 | diagram | Chapter 17:  17.2 Layered Architecture | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 371 | other | Chapter 17:  17.2 Layered Architecture | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 372 | c | Chapter 17:  17.3 Event-Driven Architecture | COMPILED_ONLY | RAN_OK | declarations only |
| 373 | c | Chapter 17:  17.3 Event-Driven Architecture | EXECUTED | RAN_OK | runs: EXTI0_IRQn |
| 374 | c | Chapter 17:  17.3 Event-Driven Architecture | EXECUTED | RAN_OK | runs: vEventDispatcherTask |
| 375 | diagram | Chapter 17:  17.4 Active Object Pattern | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 376 | c | Chapter 17:  17.4 Active Object Pattern | EXECUTED | RAN_OK | runs: vActiveObjectTask |
| 377 | diagram | Chapter 17:  17.5 Publish-Subscribe Pattern | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 378 | c | Chapter 17:  17.5 Publish-Subscribe Pattern | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 379 | c | Chapter 17:  17.6 Command Pattern and Protocol Ha | COMPILED_ONLY | RAN_OK | declarations only |
| 380 | c | Chapter 17:  17.6 Command Pattern and Protocol Ha | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 381 | c | Chapter 17:  17.6 Command Pattern and Protocol Ha | EXECUTED | RAN_OK | runs: vSerialCmdTask |
| 382 | c | Chapter 17:  17.7 State Machine Patterns | COMPILED_ONLY | RAN_OK | declarations only |
| 383 | c | Chapter 17:  17.7 State Machine Patterns | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 384 | c | Chapter 17:  17.8 Resource Management Patterns | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 385 | c | Chapter 17:  17.8 Resource Management Patterns | EXECUTED | RAN_OK | runs: vSpiBrokerTask |
| 386 | other | Chapter 17:  17.9 System Startup and Shutdown | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 387 | c | Chapter 17:  17.9 System Startup and Shutdown | EXECUTED | RAN_OK | program (main) |
| 388 | diagram | Chapter 17:  17.12 Lab 17: Designing an Event-Dri | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 389 | c | Chapter 17:  17.12 Lab 17: Designing an Event-Dri | EXECUTED | RAN_OK | program (main) |
| 390 | c | Chapter 18:  18.4 MISRA C Compliance | EXECUTED | RAN_OK | runs: vTemperatureMonitorTask |
| 391 | other | Chapter 18:  18.5 Traceability and Documentation | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 392 | diagram | Chapter 18:  18.6 V-Model Development Process | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 393 | c | Chapter 18:  18.11 Lab 18: Creating a Certificati | EXCERPT | BUILD_FAIL | does not compile even with stubs/context |
| 394 | c | Chapter 19:  19.3 FreeRTOS SMP | COMPILED_ONLY | RAN_OK | declarations only |
| 395 | c | Chapter 19:  19.3 FreeRTOS SMP | EXECUTED | RAN_OK | program (main) |
| 396 | c | Chapter 19:  19.3 FreeRTOS SMP | EXECUTED | RAN_OK | fragment in runner task |
| 397 | c | Chapter 19:  19.3 FreeRTOS SMP | EXECUTED | RAN_OK | runs: counter_init |
| 398 | diagram | Chapter 19:  19.4 AMP with FreeRTOS | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 399 | c | Chapter 19:  19.4 AMP with FreeRTOS | BOOK_ISSUE | BUILD_FAIL | does not compile even with stubs/context |
| 400 | c | Chapter 19:  19.4 AMP with FreeRTOS | BOOK_ISSUE | BUILD_FAIL | does not compile even with stubs/context |
| 401 | c | Chapter 19:  19.5 Inter-Core Communication | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 402 | c | Chapter 19:  19.5 Inter-Core Communication | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 403 | diagram | Chapter 19:  19.5 Inter-Core Communication | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 404 | diagram | Chapter 19:  19.6 Mixed-Criticality Systems | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 405 | other-rtos | Chapter 19:  19.8 QNX and Zephyr Multi-Core | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 406 | c | Chapter 19:  19.9 Common Pitfalls | EXECUTED | RAN_OK | fragment in runner task |
| 407 | c | Chapter 19:  19.10 Lab 19: AMP Inter-Core Communi | EXECUTED_HELPERS | RAN_OK | no-entry (helpers only; built+linked+booted) |
| 408 | c | Chapter 19:  19.10 Lab 19: AMP Inter-Core Communi | HW_LIMITED | FAULT | program (main) |
| 409 | c | Chapter 19:  19.10 Lab 19: AMP Inter-Core Communi | HW_LIMITED | FAULT | program (main) |
| 410 | c | Chapter 19:  19.10 Lab 19: AMP Inter-Core Communi | EXECUTED | RAN_OK | program (main) |
| 411 | c | Appendix D:   | COMPILED_ONLY | RAN_OK | declarations only |
| 412 | other | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 413 | shell | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 414 | shell | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 415 | shell | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 416 | shell | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 417 | shell | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 418 | other-rtos | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 419 | config-other | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 420 | other-rtos | Appendix F:  F.1 Windows Setup | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 421 | other | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | formula / prose / table - not code |
| 422 | other-rtos | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 423 | shell | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 424 | shell | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 425 | shell | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 426 | config-other | Appendix F:  F.2 Linux Setup (Ubuntu 22.04+) | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 427 | other-rtos | Appendix F:  F.3 macOS Setup | NOT_CODE | NOT_EXECUTABLE | QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code |
| 428 | shell | Appendix F:  F.3 macOS Setup | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 429 | config-other | Appendix F:  F.3 macOS Setup | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 430 | diagram | Appendix F:  F.5 Project Template | NOT_CODE | NOT_EXECUTABLE | ASCII diagram / table - not code |
| 431 | cmake | Appendix F:  F.5 Project Template | NOT_CODE | NOT_EXECUTABLE | CMake build script - build system, not FreeRTOS code |
| 432 | shell | Appendix F:  F.5 Project Template | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 433 | config-other | Appendix F:  F.5 Project Template | NOT_CODE | NOT_EXECUTABLE | JSON/YAML/shell configuration - not FreeRTOS code |
| 434 | shell | Appendix F:  F.6 FreeRTOS Source Integration | NOT_CODE | NOT_EXECUTABLE | host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code |
| 435 | cmake | Appendix F:  F.6 FreeRTOS Source Integration | NOT_CODE | NOT_EXECUTABLE | CMake build script - build system, not FreeRTOS code |
| 436 | c | Appendix F:  F.6 FreeRTOS Source Integration | COMPILED_ONLY | RAN_OK | declarations only |
