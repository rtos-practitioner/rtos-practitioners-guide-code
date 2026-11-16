/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.4 ISR-Safe Queue Operations > The pxHigherPriorityTaskWoken Parameter
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
