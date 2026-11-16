/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.8 QNX and Zephyr Portability Notes > Zephyr RTOS
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
struct k_work my_work;

void my_work_handler(struct k_work *work) {
    /* Runs in work queue thread context -- can block */
}

k_work_init(&my_work, my_work_handler);
k_work_submit(&my_work);  /* Submit to system work queue */
