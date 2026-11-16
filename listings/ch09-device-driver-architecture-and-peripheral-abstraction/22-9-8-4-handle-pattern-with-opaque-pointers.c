/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.8 Driver Design Patterns > 9.8.4 Handle Pattern with Opaque Pointers
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Public header */
typedef struct uart_driver *uart_handle_t;

uart_handle_t uart_open(uart_port_t port, uint32_t baudrate);
int           uart_write(uart_handle_t h, const uint8_t *data,
                          size_t len, uint32_t timeout_ms);
void          uart_close(uart_handle_t h);
