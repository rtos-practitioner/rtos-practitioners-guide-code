/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.2 Four-Layer Driver Model > 9.2.4 Layer 4: Application Interface
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Application-facing API (hardware-independent) */
int uart_write(uart_port_t port, const uint8_t *data,
               size_t len, uint32_t timeout_ms);
int uart_read(uart_port_t port, uint8_t *buf,
              size_t len, uint32_t timeout_ms);
