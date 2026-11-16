# Chapter 5: Semaphores, Mutexes, and Resource Sharing

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-5-1-1-a-concrete-race-read-modify-write.c](01-5-1-1-a-concrete-race-read-modify-write.c) | 5.1 The Critical Section Problem > 5.1.1 A Concrete Race: Read-Modify-Write | executed |
| [02-5-1-1-a-concrete-race-read-modify-write.s](02-5-1-1-a-concrete-race-read-modify-write.s) | 5.1 The Critical Section Problem > 5.1.1 A Concrete Race: Read-Modify-Write | not run (host command / other RTOS / configuration) |
| [03-5-1-2-why-disabling-interrupts-is-insufficient.c](03-5-1-2-why-disabling-interrupts-is-insufficient.c) | 5.1 The Critical Section Problem > 5.1.2 Why Disabling Interrupts Is Insufficient | executed |
| [04-5-2-1-creation-and-api.c](04-5-2-1-creation-and-api.c) | 5.2 Binary Semaphores > 5.2.1 Creation and API | compiled, linked and booted (declarations only) |
| [05-5-2-1-creation-and-api.c](05-5-2-1-creation-and-api.c) | 5.2 Binary Semaphores > 5.2.1 Creation and API | compiled, linked and booted (declarations only) |
| [06-5-2-1-creation-and-api.c](06-5-2-1-creation-and-api.c) | 5.2 Binary Semaphores > 5.2.1 Creation and API | compiled, linked and booted (declarations only) |
| [07-5-2-2-primary-use-isr-to-task-synchronization-deferred-processin.c](07-5-2-2-primary-use-isr-to-task-synchronization-deferred-processin.c) | 5.2 Binary Semaphores > 5.2.2 Primary Use: ISR-to-Task Synchronization (Deferred Processing) | executed |
| [08-5-3-counting-semaphores.c](08-5-3-counting-semaphores.c) | 5.3 Counting Semaphores | compiled, linked and booted (declarations only) |
| [09-5-3-2-use-case-2-resource-pool-management.c](09-5-3-2-use-case-2-resource-pool-management.c) | 5.3 Counting Semaphores > 5.3.2 Use Case 2: Resource Pool Management | executed |
| [10-5-4-1-creation-and-api.c](10-5-4-1-creation-and-api.c) | 5.4 Mutexes and Priority Inheritance > 5.4.1 Creation and API | compiled, linked and booted (declarations only) |
| [11-5-4-4-recursive-mutexes.c](11-5-4-4-recursive-mutexes.c) | 5.4 Mutexes and Priority Inheritance > 5.4.4 Recursive Mutexes | compiled, linked and booted (declarations only) |
| [12-5-5-1-taskenter-critical-taskexit-critical.c](12-5-5-1-taskenter-critical-taskexit-critical.c) | 5.5 Critical Sections and Interrupt Masking > 5.5.1 taskENTER_CRITICAL / taskEXIT_CRITICAL | executed |
| [13-5-5-1-taskenter-critical-taskexit-critical.c](13-5-5-1-taskenter-critical-taskexit-critical.c) | 5.5 Critical Sections and Interrupt Masking > 5.5.1 taskENTER_CRITICAL / taskEXIT_CRITICAL | executed |
| [14-5-5-2-isr-versions.c](14-5-5-2-isr-versions.c) | 5.5 Critical Sections and Interrupt Masking > 5.5.2 ISR Versions | executed (helper functions called with test arguments) |
| [15-5-5-3-scheduler-suspension.c](15-5-5-3-scheduler-suspension.c) | 5.5 Critical Sections and Interrupt Masking > 5.5.3 Scheduler Suspension | executed |
| [16-5-6-3-code-example-deadlock-and-its-fix.c](16-5-6-3-code-example-deadlock-and-its-fix.c) | 5.6 Deadlock > 5.6.3 Code Example: Deadlock and Its Fix | executed |
| [17-5-6-3-code-example-deadlock-and-its-fix.c](17-5-6-3-code-example-deadlock-and-its-fix.c) | 5.6 Deadlock > 5.6.3 Code Example: Deadlock and Its Fix | executed |
| [18-5-6-4-prevention-strategies.c](18-5-6-4-prevention-strategies.c) | 5.6 Deadlock > 5.6.4 Prevention Strategies | executed |
| [19-5-7-1-creation.c](19-5-7-1-creation.c) | 5.7 Event Groups > 5.7.1 Creation | compiled, linked and booted (declarations only) |
| [20-5-7-2-setting-and-waiting.c](20-5-7-2-setting-and-waiting.c) | 5.7 Event Groups > 5.7.2 Setting and Waiting | compiled, linked and booted (declarations only) |
| [21-5-7-3-synchronization-rendezvous-xeventgroupsync.c](21-5-7-3-synchronization-rendezvous-xeventgroupsync.c) | 5.7 Event Groups > 5.7.3 Synchronization Rendezvous: xEventGroupSync | compiled, linked and booted (declarations only) |
| [22-5-7-4-use-case-multi-subsystem-initialization-barrier.c](22-5-7-4-use-case-multi-subsystem-initialization-barrier.c) | 5.7 Event Groups > 5.7.4 Use Case: Multi-Subsystem Initialization Barrier | executed |
| [23-5-11-complete-code-listing.c](23-5-11-complete-code-listing.c) | 5.11 Lab 5: Priority Inversion Demonstration and Cure > Complete Code Listing | executed |
