/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.6 Message Buffers and Stream Buffers > Stream Buffers
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
StreamBufferHandle_t xStreamBuf = xStreamBufferCreate(
    256,   /* total buffer size in bytes */
    1      /* trigger level: minimum bytes to unblock reader */
);
