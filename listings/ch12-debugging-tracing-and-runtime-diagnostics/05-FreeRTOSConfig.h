/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.4 FreeRTOS Trace Hooks > 12.4.3 Hooking the Kernel
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* In FreeRTOSConfig.h */
extern void FR_Record(uint8_t event, uint8_t task_id, uint16_t detail);

#define traceTASK_SWITCHED_IN()                                       \
    FR_Record(0x01,                                                   \
              (uint8_t)(pxCurrentTCB->uxTaskNumber),                  \
              (uint16_t)(pxCurrentTCB->uxPriority))

#define traceTASK_SWITCHED_OUT()                                      \
    FR_Record(0x02,                                                   \
              (uint8_t)(pxCurrentTCB->uxTaskNumber),                  \
              (uint16_t)(pxCurrentTCB->uxPriority))

#define traceQUEUE_SEND(pxQueue)                                     \
    FR_Record(0x03, 0,                                                \
              (uint16_t)(uxQueueMessagesWaiting(pxQueue)))

#define traceQUEUE_RECEIVE(pxQueue)                                   \
    FR_Record(0x04, 0,                                                \
              (uint16_t)(uxQueueMessagesWaiting(pxQueue)))
