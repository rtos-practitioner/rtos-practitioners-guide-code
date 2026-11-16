# Chapter 8: Interrupt Management and ISR Design

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-8-3-configkernel-interrupt-priority.c](01-8-3-configkernel-interrupt-priority.c) | 8.3 FreeRTOS Interrupt Priority Model > configKERNEL_INTERRUPT_PRIORITY | compiled, linked and booted (declarations only) |
| [02-8-3-configmax-syscall-interrupt-priority.c](02-8-3-configmax-syscall-interrupt-priority.c) | 8.3 FreeRTOS Interrupt Priority Model > configMAX_SYSCALL_INTERRUPT_PRIORITY | compiled, linked and booted (declarations only) |
| [03-8-3-configassert-validation.c](03-8-3-configassert-validation.c) | 8.3 FreeRTOS Interrupt Priority Model > configASSERT Validation | compiled, linked and booted (declarations only) |
| [04-8-4-pattern-1-minimal-isr-defer-everything.c](04-8-4-pattern-1-minimal-isr-defer-everything.c) | 8.4 ISR Design Patterns > Pattern 1: Minimal ISR — Defer Everything | executed |
| [05-8-4-pattern-2-isr-with-immediate-data-capture.c](05-8-4-pattern-2-isr-with-immediate-data-capture.c) | 8.4 ISR Design Patterns > Pattern 2: ISR with Immediate Data Capture | executed |
| [06-8-4-pattern-3-isr-with-critical-immediate-action.c](06-8-4-pattern-3-isr-with-critical-immediate-action.c) | 8.4 ISR Design Patterns > Pattern 3: ISR with Critical Immediate Action | executed |
| [07-8-5-portyield-from-isr.c](07-8-5-portyield-from-isr.c) | 8.5 The FromISR API Family > portYIELD_FROM_ISR | executed |
| [08-8-5-multi-operation-isr-pattern.c](08-8-5-multi-operation-isr-pattern.c) | 8.5 The FromISR API Family > Multi-Operation ISR Pattern | executed |
| [09-8-6-freertos-critical-sections-and-basepri.c](09-8-6-freertos-critical-sections-and-basepri.c) | 8.6 Interrupt Nesting > FreeRTOS Critical Sections and BASEPRI | executed (helper functions called with test arguments) |
| [10-STM32F407VGTx_FLASH.ld](10-STM32F407VGTx_FLASH.ld) | 8.6 Interrupt Nesting > Worst-Case Stack Calculation | not run (host command / other RTOS / configuration) |
| [11-startup_stm32f407xx.s](11-startup_stm32f407xx.s) | 8.6 Interrupt Nesting > Worst-Case Stack Calculation | not run (host command / other RTOS / configuration) |
| [12-8-7-uart-dma-receive-with-idle-line-detection.c](12-8-7-uart-dma-receive-with-idle-line-detection.c) | 8.7 Direct Memory Access (DMA) Integration > UART DMA Receive with Idle Line Detection | executed |
| [13-8-8-measurement-techniques.c](13-8-8-measurement-techniques.c) | 8.8 Measuring and Reducing Interrupt Latency > Measurement Techniques | executed |
| [14-8-8-measurement-techniques.c](14-8-8-measurement-techniques.c) | 8.8 Measuring and Reducing Interrupt Latency > Measurement Techniques | executed |
| [15-8-8-reduction-strategies.c](15-8-8-reduction-strategies.c) | 8.8 Measuring and Reducing Interrupt Latency > Reduction Strategies | executed |
| [16-8-10-common-pitfalls.c](16-8-10-common-pitfalls.c) | 8.10 Common Pitfalls | compiled, linked and booted (declarations only) |
| [17-8-11-complete-code-listing.c](17-8-11-complete-code-listing.c) | 8.11 Lab 8: Interrupt Latency Measurement and ISR Optimization > Complete Code Listing | executed |
