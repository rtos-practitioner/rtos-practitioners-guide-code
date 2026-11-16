/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.8 Driver Design Patterns > 9.8.2 Callback Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef void (*uart_rx_callback_t)(uart_port_t port,
                                    const uint8_t *data,
                                    size_t len);

void uart_register_rx_callback(uart_port_t port,
                                uart_rx_callback_t cb);
