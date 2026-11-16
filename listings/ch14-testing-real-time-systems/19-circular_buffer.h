/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.11 Lab 14: Building a Test Pipeline > Part 1: Unit Tests for a Circular Buffer
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* src/circular_buffer.h */
#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define CBUF_CAPACITY  64

typedef struct {
    uint8_t data[CBUF_CAPACITY];
    int     head;
    int     tail;
    int     count;
} CircularBuffer;

void    cbuf_init(CircularBuffer *cb);
bool    cbuf_push(CircularBuffer *cb, uint8_t byte);
bool    cbuf_pop(CircularBuffer *cb, uint8_t *byte);
int     cbuf_count(const CircularBuffer *cb);
bool    cbuf_is_empty(const CircularBuffer *cb);
bool    cbuf_is_full(const CircularBuffer *cb);

#endif
