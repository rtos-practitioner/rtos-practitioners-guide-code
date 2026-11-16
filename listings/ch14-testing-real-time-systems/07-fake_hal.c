/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.3 The Fake HAL Pattern (Expanded) > Simulating Timing
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* fake_hal.c */
static TickType_t fake_tick_count = 0;

TickType_t hal_get_tick(void) {
    return fake_tick_count;
}

/* Test control API — not in hal_api.h, only in fake_hal_test.h */
void fake_hal_set_tick(TickType_t t) {
    fake_tick_count = t;
}

void fake_hal_advance_tick(TickType_t delta) {
    fake_tick_count += delta;
}
