/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.5 System Testing on Target > Timing Verification
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
bool test_control_loop_timing(void) {
    HAL_GPIO_WritePin(TEST_GPIO_PORT, TEST_PIN, GPIO_PIN_SET);
    control_loop_execute();  /* the function under test */
    HAL_GPIO_WritePin(TEST_GPIO_PORT, TEST_PIN, GPIO_PIN_RESET);
    /* External measurement evaluates pulse width */
    return true;  /* pass/fail determined externally */
}
