/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.3 SysTick Timer and RTOS Tick > 11.3.1 SysTick Architecture
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Reload value for 1 ms tick at 168 MHz SYSCLK */
#define configCPU_CLOCK_HZ          168000000UL
#define configTICK_RATE_HZ          1000

/* portNVIC_SYSTICK_LOAD_REG = SYSCLK / TICK_RATE - 1 */
portNVIC_SYSTICK_LOAD_REG = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1;
/*                        = 168000000 / 1000 - 1 = 167999                 */
