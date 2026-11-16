/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.1 Shared Memory Layout
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
/* ipc_shared.h — shared between M7 and M4 projects */

#ifndef IPC_SHARED_H
#define IPC_SHARED_H

#include <stdint.h>

#define IPC_RING_SIZE     128   /* Must be power of 2 */
#define IPC_RING_MASK     (IPC_RING_SIZE - 1)
#define HSEM_ID_IPC_NOTIFY  0   /* HSEM slot for IPC */

typedef struct {
    uint32_t timestamp_us;
    int16_t  sensor_value;
    uint16_t sequence;
} __attribute__((packed)) SensorSample_t;

typedef struct {
    volatile uint32_t write_idx;
    volatile uint32_t read_idx;
    SensorSample_t    samples[IPC_RING_SIZE];
} IPC_Ring_t;

typedef struct {
    volatile uint32_t initialized;   /* Set to 0xDEADBEEF by M7 */
    volatile uint32_t m4_ready;      /* Set to 0xCAFECAFE by M4 */
    IPC_Ring_t        sensor_ring;
    volatile uint32_t latency_timestamp; /* For latency measurement */
} SharedIPC_t;

/* Ring buffer operations — inlined for both cores */
static inline int ipc_ring_push(volatile IPC_Ring_t *ring,
                                const SensorSample_t *sample)
{
    uint32_t w = ring->write_idx;
    uint32_t next_w = (w + 1) & IPC_RING_MASK;
    if (next_w == ring->read_idx) return -1; /* Full */
    __DMB();
    ring->samples[w] = *sample;
    __DMB();
    ring->write_idx = next_w;
    return 0;
}

static inline int ipc_ring_pop(volatile IPC_Ring_t *ring,
                               SensorSample_t *sample)
{
    uint32_t r = ring->read_idx;
    if (r == ring->write_idx) return -1; /* Empty */
    __DMB();
    *sample = ring->samples[r];
    __DMB();
    ring->read_idx = (r + 1) & IPC_RING_MASK;
    return 0;
}

#endif /* IPC_SHARED_H */
