# Chapter 19: Multi-Core RTOS: SMP and AMP Architectures

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-19-3-1-configuration.c](01-19-3-1-configuration.c) | 19.3 FreeRTOS SMP > 19.3.1 Configuration | compiled, linked and booted (declarations only) |
| [02-19-3-2-task-affinity.c](02-19-3-2-task-affinity.c) | 19.3 FreeRTOS SMP > 19.3.2 Task Affinity | executed |
| [03-19-3-3-scheduler-and-synchronization-changes.c](03-19-3-3-scheduler-and-synchronization-changes.c) | 19.3 FreeRTOS SMP > 19.3.3 Scheduler and Synchronization Changes | executed |
| [04-19-3-3-scheduler-and-synchronization-changes.c](04-19-3-3-scheduler-and-synchronization-changes.c) | 19.3 FreeRTOS SMP > 19.3.3 Scheduler and Synchronization Changes | executed |
| [05-amp_ipc.h](05-amp_ipc.h) | 19.4 AMP with FreeRTOS > 19.4.1 Memory Map Separation | changed since the reference run - re-run tools/run_suite.py |
| [06-19-4-2-boot-sequence.c](06-19-4-2-boot-sequence.c) | 19.4 AMP with FreeRTOS > 19.4.2 Boot Sequence | changed since the reference run - re-run tools/run_suite.py |
| [07-19-4-2-boot-sequence.c](07-19-4-2-boot-sequence.c) | 19.4 AMP with FreeRTOS > 19.4.2 Boot Sequence | changed since the reference run - re-run tools/run_suite.py |
| [08-19-5-1-shared-memory.c](08-19-5-1-shared-memory.c) | 19.5 Inter-Core Communication > 19.5.1 Shared Memory | executed (helper functions called with test arguments) |
| [09-19-5-2-lock-free-spsc-ring-buffer.c](09-19-5-2-lock-free-spsc-ring-buffer.c) | 19.5 Inter-Core Communication > 19.5.2 Lock-Free SPSC Ring Buffer | executed (helper functions called with test arguments) |
| [10-19-8-2-zephyr-smp-and-ipc.c](10-19-8-2-zephyr-smp-and-ipc.c) | 19.8 QNX and Zephyr Multi-Core > 19.8.2 Zephyr SMP and IPC | not run (host command / other RTOS / configuration) |
| [11-19-9-pitfall-1-false-sharing-on-cache-lines.c](11-19-9-pitfall-1-false-sharing-on-cache-lines.c) | 19.9 Common Pitfalls > Pitfall 1: False Sharing on Cache Lines | executed |
| [12-ipc_shared.h](12-ipc_shared.h) | 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.1 Shared Memory Layout | changed since the reference run - re-run tools/run_suite.py |
| [13-main_m7.c](13-main_m7.c) | 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.2 Core M7 Firmware (Producer) | compiled; needs real hardware (not modelled by QEMU) |
| [14-main_m4.c](14-main_m4.c) | 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.3 Core M4 Firmware (Consumer) | compiled; needs real hardware (not modelled by QEMU) |
| [15-19-10-4-single-core-simulation-variant.c](15-19-10-4-single-core-simulation-variant.c) | 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.4 Single-Core Simulation Variant | executed |
