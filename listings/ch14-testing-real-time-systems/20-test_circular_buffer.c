/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.11 Lab 14: Building a Test Pipeline > Part 1: Unit Tests for a Circular Buffer
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* tests/test_circular_buffer.c */
#include "unity.h"
#include "circular_buffer.h"

static CircularBuffer cb;

void setUp(void)    { cbuf_init(&cb); }
void tearDown(void) { }

void test_new_buffer_is_empty(void) {
    TEST_ASSERT_TRUE(cbuf_is_empty(&cb));
    TEST_ASSERT_EQUAL_INT(0, cbuf_count(&cb));
}

void test_push_pop_single_byte(void) {
    TEST_ASSERT_TRUE(cbuf_push(&cb, 0xAB));
    TEST_ASSERT_EQUAL_INT(1, cbuf_count(&cb));
    uint8_t val;
    TEST_ASSERT_TRUE(cbuf_pop(&cb, &val));
    TEST_ASSERT_EQUAL_HEX8(0xAB, val);
    TEST_ASSERT_TRUE(cbuf_is_empty(&cb));
}

void test_fifo_ordering(void) {
    cbuf_push(&cb, 1);
    cbuf_push(&cb, 2);
    cbuf_push(&cb, 3);
    uint8_t val;
    cbuf_pop(&cb, &val); TEST_ASSERT_EQUAL_UINT8(1, val);
    cbuf_pop(&cb, &val); TEST_ASSERT_EQUAL_UINT8(2, val);
    cbuf_pop(&cb, &val); TEST_ASSERT_EQUAL_UINT8(3, val);
}

void test_full_buffer_rejects_push(void) {
    for (int i = 0; i < CBUF_CAPACITY; i++) {
        TEST_ASSERT_TRUE(cbuf_push(&cb, (uint8_t)i));
    }
    TEST_ASSERT_TRUE(cbuf_is_full(&cb));
    TEST_ASSERT_FALSE(cbuf_push(&cb, 0xFF));  /* rejected */
}

void test_wraparound(void) {
    /* Fill half, drain half, fill again to force wraparound */
    for (int i = 0; i < CBUF_CAPACITY / 2; i++) {
        cbuf_push(&cb, (uint8_t)i);
    }
    uint8_t tmp;
    for (int i = 0; i < CBUF_CAPACITY / 2; i++) {
        cbuf_pop(&cb, &tmp);
    }
    /* Now head and tail are at midpoint — push full capacity */
    for (int i = 0; i < CBUF_CAPACITY; i++) {
        TEST_ASSERT_TRUE(cbuf_push(&cb, (uint8_t)(i + 100)));
    }
    TEST_ASSERT_TRUE(cbuf_is_full(&cb));
    uint8_t val;
    cbuf_pop(&cb, &val);
    TEST_ASSERT_EQUAL_UINT8(100, val);  /* first pushed after drain */
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_new_buffer_is_empty);
    RUN_TEST(test_push_pop_single_byte);
    RUN_TEST(test_fifo_ordering);
    RUN_TEST(test_full_buffer_rejects_push);
    RUN_TEST(test_wraparound);
    return UNITY_END();
}
