/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.4 FreeRTOS Trace Hooks > 12.4.2 The Flight Recorder Pattern
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* flight_recorder.h */
#ifndef FLIGHT_RECORDER_H
#define FLIGHT_RECORDER_H

#include <stdint.h>

/* Event types */
#define FR_EVT_TASK_SWITCH_IN   0x01
#define FR_EVT_TASK_SWITCH_OUT  0x02
#define FR_EVT_QUEUE_SEND       0x03
#define FR_EVT_QUEUE_RECV       0x04
#define FR_EVT_BLOCK_ON_RECV    0x05
#define FR_EVT_ISR_ENTER        0x06
#define FR_EVT_ISR_EXIT         0x07
#define FR_EVT_HARD_FAULT       0x08
#define FR_EVT_STACK_LOW        0x09

#define FR_BUFFER_SIZE          256  /* must be power of 2 */

typedef struct {
    uint32_t timestamp;   /* DWT cycle count */
    uint8_t  event;       /* FR_EVT_xxx */
    uint8_t  task_id;     /* task number (from TCB) */
    uint16_t detail;      /* event-specific data */
} FlightRecordEntry;

typedef struct {
    uint32_t          magic;    /* FR_MAGIC for validity check */
    uint32_t          head;     /* next write index */
    uint32_t          count;    /* total events written (wraps) */
    FlightRecordEntry entries[FR_BUFFER_SIZE];
} FlightRecorder;

void     FR_Init(void);
void     FR_Record(uint8_t event, uint8_t task_id, uint16_t detail);
void     FR_Dump(void);

#endif
