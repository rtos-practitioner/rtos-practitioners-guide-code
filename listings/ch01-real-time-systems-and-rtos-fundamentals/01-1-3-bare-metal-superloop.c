/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 1: Real-Time Systems and RTOS Fundamentals
 * 1.3 Bare Metal vs RTOS vs Linux > Bare Metal (Superloop)
 * Verification: bare-metal super-loop (no scheduler by design)
 * SPDX-License-Identifier: MIT
 */
int main(void) {
    system_init();

    for (;;) {
        read_sensors();
        run_control();
        update_display();
        check_communications();
    }
}
