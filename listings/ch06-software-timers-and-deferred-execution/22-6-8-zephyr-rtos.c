/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.8 QNX and Zephyr Portability Notes > Zephyr RTOS
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
struct k_work_delayable my_dwork;
k_work_init_delayable(&my_dwork, my_work_handler);
k_work_schedule(&my_dwork, K_MSEC(50));  /* Execute after 50 ms */
