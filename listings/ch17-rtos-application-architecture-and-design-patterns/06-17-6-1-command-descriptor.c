/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.6 Command Pattern and Protocol Handlers > 17.6.1 Command Descriptor
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
typedef void (*CmdHandler_t)(const uint8_t *pucPayload,
                              uint16_t usLength);

typedef struct {
    uint8_t       ucCmdId;
    uint8_t       ucExpectedLen;   /* 0 = variable length */
    CmdHandler_t  pfHandler;
    const char   *pcName;          /* For debug logging */
} CmdDescriptor_t;
