/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.8 Driver Design Patterns > 9.8.3 Event-Driven Pattern
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    uart_port_t port;
    enum { UART_EVT_RX_DATA, UART_EVT_TX_DONE,
           UART_EVT_ERROR } type;
    uint16_t    data_len;
} uart_event_t;

/* Driver posts events; application task receives them */
uart_event_t event = { .port = port, .type = UART_EVT_RX_DATA,
                       .data_len = len };
xQueueSend(app_event_queue, &event, 0);
