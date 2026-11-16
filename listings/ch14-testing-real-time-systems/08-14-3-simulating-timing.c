/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.3 The Fake HAL Pattern (Expanded) > Simulating Timing
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void test_sensor_timeout_detected(void) {
    fake_hal_set_tick(0);
    sensor_start_read(&sensor);
    fake_hal_advance_tick(SENSOR_TIMEOUT_TICKS + 1);
    sensor_poll(&sensor);
    TEST_ASSERT_EQUAL(SENSOR_STATE_TIMEOUT, sensor_get_state(&sensor));
}
