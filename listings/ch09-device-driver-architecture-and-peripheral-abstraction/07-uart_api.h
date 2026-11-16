/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.3 UART Driver Case Study > 9.3.4 Layer 4: Application Interface
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* uart_api.h -- Application interface */
#ifndef UART_API_H
#define UART_API_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    UART_PORT_DEBUG = 0,  /* Maps to USART2 on STM32F407-DISC1 */
    UART_PORT_GPS,        /* Maps to USART1 or USART3           */
    UART_PORT_COUNT
} uart_port_t;

/* Initialize a UART port. Returns 0 on success. */
int uart_init(uart_port_t port, uint32_t baudrate);

/* Write data. Returns bytes written, or negative on error. */
int uart_write(uart_port_t port, const uint8_t *data,
               size_t len, uint32_t timeout_ms);

/* Read data. Returns bytes read, or negative on error. */
int uart_read(uart_port_t port, uint8_t *buf,
              size_t len, uint32_t timeout_ms);

#endif /* UART_API_H */
