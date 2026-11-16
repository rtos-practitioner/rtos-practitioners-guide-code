/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.2 Unit Testing on Host > Code Example: Unity Test for a Moving Average Filter
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* src/moving_average.h */
#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdint.h>

#define MA_WINDOW_SIZE  8

typedef struct {
    int32_t buffer[MA_WINDOW_SIZE];
    int     index;
    int     count;
    int64_t sum;
} MovingAverage;

void     ma_init(MovingAverage *ma);
int32_t  ma_update(MovingAverage *ma, int32_t sample);

#endif
