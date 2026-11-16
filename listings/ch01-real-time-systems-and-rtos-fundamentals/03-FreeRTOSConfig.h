/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 1: Real-Time Systems and RTOS Fundamentals
 * 1.5 FreeRTOS Architecture Overview > The Configuration Model
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h - Critical Configuration Parameters */

#define configCPU_CLOCK_HZ              168000000   /* STM32F407 at 168 MHz */
#define configTICK_RATE_HZ              1000        /* 1 ms tick period */
#define configMAX_PRIORITIES            8           /* Priority levels 0-7 */
#define configMINIMAL_STACK_SIZE        128         /* Idle task stack (words) */
#define configTOTAL_HEAP_SIZE           (32 * 1024) /* 32 KB kernel heap */
#define configUSE_PREEMPTION            1           /* Preemptive scheduler */
#define configUSE_TIME_SLICING          1           /* Round-robin at same priority */
#define configUSE_TICKLESS_IDLE         0           /* Disable for initial work */

/* Safety and debug */
#define configCHECK_FOR_STACK_OVERFLOW  2           /* Method 2: pattern check */
#define configASSERT(x)                 if (!(x)) { taskDISABLE_INTERRUPTS(); for(;;); }
#define configUSE_TRACE_FACILITY        1           /* Enable trace macros */
#define configGENERATE_RUN_TIME_STATS   1           /* CPU usage statistics */

/* Cortex-M interrupt configuration */
#define configPRIO_BITS                         4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY     15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
