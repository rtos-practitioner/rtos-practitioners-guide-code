# Chapter 7: Memory Management in Resource-Constrained Systems

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-7-2-freertos-heap-implementations.c](01-7-2-freertos-heap-implementations.c) | 7.2 FreeRTOS Heap Implementations | compiled, linked and booted (declarations only) |
| [02-7-2-heap-5-spans-multiple-non-contiguous-regions.c](02-7-2-heap-5-spans-multiple-non-contiguous-regions.c) | 7.2 FreeRTOS Heap Implementations > heap_5: Spans Multiple Non-Contiguous Regions | compiled; needs real hardware (not modelled by QEMU) |
| [03-FreeRTOSConfig.h](03-FreeRTOSConfig.h) | 7.2 FreeRTOS Heap Implementations > Placement of ucHeap in CCM RAM | compiled, linked and booted (declarations only) |
| [04-7-2-placement-of-ucheap-in-ccm-ram.ld](04-7-2-placement-of-ucheap-in-ccm-ram.ld) | 7.2 FreeRTOS Heap Implementations > Placement of ucHeap in CCM RAM | not run (host command / other RTOS / configuration) |
| [05-7-3-how-static-allocation-works.c](05-7-3-how-static-allocation-works.c) | 7.3 Static vs Dynamic Allocation > How Static Allocation Works | executed |
| [06-7-3-required-callbacks.c](06-7-3-required-callbacks.c) | 7.3 Static vs Dynamic Allocation > Required Callbacks | compiled, linked and booted (declarations only) |
| [07-FreeRTOSConfig.h](07-FreeRTOSConfig.h) | 7.3 Static vs Dynamic Allocation > Example: Fully Static System with Zero Heap | compiled, linked and booted (declarations only) |
| [08-7-4-stack-overflow-detection.c](08-7-4-stack-overflow-detection.c) | 7.4 Stack Sizing and Overflow Detection > Stack Overflow Detection | compiled, linked and booted (declarations only) |
| [09-7-4-runtime-high-water-mark-monitoring.c](09-7-4-runtime-high-water-mark-monitoring.c) | 7.4 Stack Sizing and Overflow Detection > Runtime High-Water Mark Monitoring | executed |
| [10-7-5-freertos-mpu-port.c](10-7-5-freertos-mpu-port.c) | 7.5 Memory Protection Unit (MPU) > FreeRTOS-MPU Port | needs the FreeRTOS-MPU port (not built by the harness) |
| [11-7-6-detection.c](11-7-6-detection.c) | 7.6 Fragmentation Analysis and Prevention > Detection | executed |
| [12-7-6-prevention-strategies.c](12-7-6-prevention-strategies.c) | 7.6 Fragmentation Analysis and Prevention > Prevention Strategies | executed (helper functions called with test arguments) |
| [13-7-7-heap-allocation-tracing.c](13-7-7-heap-allocation-tracing.c) | 7.7 Memory Debugging Techniques > Heap Allocation Tracing | compiled, linked and booted (declarations only) |
| [14-7-7-memory-poisoning.c](14-7-7-memory-poisoning.c) | 7.7 Memory Debugging Techniques > Memory Poisoning | compiled, linked and booted (declarations only) |
| [15-7-10-freertosconfig-h-settings.c](15-7-10-freertosconfig-h-settings.c) | 7.10 Lab 7: Memory Budget and Overflow Detection > FreeRTOSConfig.h Settings | compiled, linked and booted (declarations only) |
| [16-lab7_memory.c](16-lab7_memory.c) | 7.10 Lab 7: Memory Budget and Overflow Detection > Complete Source Code | executed |
