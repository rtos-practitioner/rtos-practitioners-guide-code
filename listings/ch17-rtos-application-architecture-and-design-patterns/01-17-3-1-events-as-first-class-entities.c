/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.3 Event-Driven Architecture > 17.3.1 Events as First-Class Entities
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
typedef enum {
    EVT_BUTTON_PRESSED,
    EVT_BUTTON_RELEASED,
    EVT_SENSOR_DATA_READY,
    EVT_TIMER_EXPIRED,
    EVT_UART_RX_COMPLETE,
    EVT_STATE_CHANGED,
    EVT_ERROR_DETECTED
} EventType_t;

typedef struct {
    EventType_t eType;
    uint32_t    ulTimestamp;
    uint32_t    ulPayload;
} Event_t;
