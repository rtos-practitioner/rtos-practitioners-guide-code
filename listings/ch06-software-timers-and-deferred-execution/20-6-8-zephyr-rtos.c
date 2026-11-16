/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.8 QNX and Zephyr Portability Notes > Zephyr RTOS
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* Zephyr timer */
void my_expiry_fn(struct k_timer *timer) {
    /* Called at each expiry -- runs in ISR context! */
}
void my_stop_fn(struct k_timer *timer) {
    /* Called when timer is stopped */
}
K_TIMER_DEFINE(my_timer, my_expiry_fn, my_stop_fn);

k_timer_start(&my_timer, K_MSEC(100), K_MSEC(100));  /* 100 ms periodic */
