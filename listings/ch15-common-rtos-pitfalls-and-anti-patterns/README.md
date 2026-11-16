# Chapter 15: Common RTOS Pitfalls and Anti-Patterns

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-15-2-pitfall-unbounded-priority-inversion-wrong.c](01-15-2-pitfall-unbounded-priority-inversion-wrong.c) | 15.2 Pitfall: Unbounded Priority Inversion > Code Example | executed |
| [02-15-2-pitfall-unbounded-priority-inversion-correct.c](02-15-2-pitfall-unbounded-priority-inversion-correct.c) | 15.2 Pitfall: Unbounded Priority Inversion > Code Example | executed |
| [03-15-3-pitfall-deadlock.c](03-15-3-pitfall-deadlock.c) | 15.3 Pitfall: Deadlock > Code Example | executed |
| [04-15-3-pitfall-deadlock.c](04-15-3-pitfall-deadlock.c) | 15.3 Pitfall: Deadlock > Code Example | executed |
| [05-15-4-pitfall-stack-overflow.c](05-15-4-pitfall-stack-overflow.c) | 15.4 Pitfall: Stack Overflow > Code Example | executed |
| [06-15-4-pitfall-stack-overflow.c](06-15-4-pitfall-stack-overflow.c) | 15.4 Pitfall: Stack Overflow > Code Example | executed |
| [07-15-5-pitfall-race-conditions-on-shared-data.c](07-15-5-pitfall-race-conditions-on-shared-data.c) | 15.5 Pitfall: Race Conditions on Shared Data > Code Example | executed |
| [08-15-5-pitfall-race-conditions-on-shared-data.c](08-15-5-pitfall-race-conditions-on-shared-data.c) | 15.5 Pitfall: Race Conditions on Shared Data > Code Example | executed |
| [09-15-6-pitfall-isr-calling-non-thread-safe-apis.c](09-15-6-pitfall-isr-calling-non-thread-safe-apis.c) | 15.6 Pitfall: ISR Calling Non-Thread-Safe APIs > Code Example | anti-pattern: fails exactly as the book describes |
| [10-15-6-pitfall-isr-calling-non-thread-safe-apis.c](10-15-6-pitfall-isr-calling-non-thread-safe-apis.c) | 15.6 Pitfall: ISR Calling Non-Thread-Safe APIs > Code Example | executed |
| [11-15-7-pitfall-polling-instead-of-blocking.c](11-15-7-pitfall-polling-instead-of-blocking.c) | 15.7 Pitfall: Polling Instead of Blocking > Code Example | executed |
| [12-15-7-pitfall-polling-instead-of-blocking.c](12-15-7-pitfall-polling-instead-of-blocking.c) | 15.7 Pitfall: Polling Instead of Blocking > Code Example | executed |
| [13-15-8-pitfall-misusing-vtaskdelay-for-periodic-timing.c](13-15-8-pitfall-misusing-vtaskdelay-for-periodic-timing.c) | 15.8 Pitfall: Misusing vTaskDelay for Periodic Timing > Code Example | executed |
| [14-15-8-pitfall-misusing-vtaskdelay-for-periodic-timing.c](14-15-8-pitfall-misusing-vtaskdelay-for-periodic-timing.c) | 15.8 Pitfall: Misusing vTaskDelay for Periodic Timing > Code Example | executed |
| [15-15-9-example-priority-band-structure.c](15-15-9-example-priority-band-structure.c) | 15.9 Pitfall: Unbounded Priority Levels > Example: Priority Band Structure | executed |
| [16-15-10-pitfall-memory-leaks-and-heap-fragmentation.c](16-15-10-pitfall-memory-leaks-and-heap-fragmentation.c) | 15.10 Pitfall: Memory Leaks and Heap Fragmentation > Code Example | executed |
| [17-15-10-pitfall-memory-leaks-and-heap-fragmentation.c](17-15-10-pitfall-memory-leaks-and-heap-fragmentation.c) | 15.10 Pitfall: Memory Leaks and Heap Fragmentation > Code Example | executed |
| [18-15-11-prevention-wrong.c](18-15-11-prevention-wrong.c) | 15.11 Pitfall: Starving the Idle Task > Prevention | executed |
| [19-15-12-pitfall-interrupt-priority-misconfiguration.c](19-15-12-pitfall-interrupt-priority-misconfiguration.c) | 15.12 Pitfall: Interrupt Priority Misconfiguration > Code Example | executed |
| [20-15-12-pitfall-interrupt-priority-misconfiguration.c](20-15-12-pitfall-interrupt-priority-misconfiguration.c) | 15.12 Pitfall: Interrupt Priority Misconfiguration > Code Example | executed |
| [21-15-13-pitfall-global-state-and-hidden-coupling.c](21-15-13-pitfall-global-state-and-hidden-coupling.c) | 15.13 Pitfall: Global State and Hidden Coupling > Code Example | executed |
| [22-15-13-pitfall-global-state-and-hidden-coupling.c](22-15-13-pitfall-global-state-and-hidden-coupling.c) | 15.13 Pitfall: Global State and Hidden Coupling > Code Example | executed |
| [23-15-16-exercise-1-identify-the-pitfall.c](23-15-16-exercise-1-identify-the-pitfall.c) | 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall | executed |
| [24-15-16-exercise-1-identify-the-pitfall.c](24-15-16-exercise-1-identify-the-pitfall.c) | 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall | anti-pattern: fails exactly as the book describes |
| [25-15-16-exercise-1-identify-the-pitfall.c](25-15-16-exercise-1-identify-the-pitfall.c) | 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall | changed since the reference run - re-run tools/run_suite.py |
| [26-15-16-exercise-1-identify-the-pitfall.c](26-15-16-exercise-1-identify-the-pitfall.c) | 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall | executed |
| [27-15-16-exercise-1-identify-the-pitfall.c](27-15-16-exercise-1-identify-the-pitfall.c) | 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall | executed |
