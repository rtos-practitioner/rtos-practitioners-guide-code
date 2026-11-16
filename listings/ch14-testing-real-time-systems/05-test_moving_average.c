/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.2 Unit Testing on Host > Code Example: Unity Test for a Moving Average Filter
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* tests/test_moving_average.c */
#include "unity.h"
#include "moving_average.h"

static MovingAverage ma;

void setUp(void) {
    ma_init(&ma);
}

void tearDown(void) {
    /* nothing to clean up */
}

void test_initial_output_equals_first_sample(void) {
    int32_t result = ma_update(&ma, 1000);
    TEST_ASSERT_EQUAL_INT32(1000, result);
}

void test_average_of_constant_input_equals_input(void) {
    for (int i = 0; i < MA_WINDOW_SIZE * 2; i++) {
        ma_update(&ma, 500);
    }
    TEST_ASSERT_EQUAL_INT32(500, ma_update(&ma, 500));
}

void test_step_response_converges(void) {
    /* Fill with zeros */
    for (int i = 0; i < MA_WINDOW_SIZE; i++) {
        ma_update(&ma, 0);
    }
    /* Step to 800 */
    int32_t result = 0;
    for (int i = 0; i < MA_WINDOW_SIZE; i++) {
        result = ma_update(&ma, 800);
    }
    TEST_ASSERT_EQUAL_INT32(800, result);  /* fully converged */
}

void test_negative_values_handled(void) {
    for (int i = 0; i < MA_WINDOW_SIZE; i++) {
        ma_update(&ma, -100);
    }
    TEST_ASSERT_EQUAL_INT32(-100, ma_update(&ma, -100));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_output_equals_first_sample);
    RUN_TEST(test_average_of_constant_input_equals_input);
    RUN_TEST(test_step_response_converges);
    RUN_TEST(test_negative_values_handled);
    return UNITY_END();
}
