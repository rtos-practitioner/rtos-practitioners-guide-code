/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.11 Lab 14: Building a Test Pipeline > Part 2: Fake UART HAL
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* tests/fakes/fake_uart_hal.c */
#include "hal_api.h"
#include <string.h>

static uint8_t tx_log[256];
static int     tx_log_count;
static uint8_t rx_inject[256];
static int     rx_inject_count;
static int     rx_inject_index;

void fake_uart_reset(void) {
    tx_log_count = 0;
    rx_inject_count = 0;
    rx_inject_index = 0;
}

void fake_uart_inject_rx(const uint8_t *data, int len) {
    memcpy(rx_inject, data, len);
    rx_inject_count = len;
    rx_inject_index = 0;
}

const uint8_t *fake_uart_get_tx_log(int *count) {
    *count = tx_log_count;
    return tx_log;
}

HAL_StatusTypeDef hal_uart_transmit(const uint8_t *data, uint16_t len) {
    for (int i = 0; i < len; i++) {
        tx_log[tx_log_count++] = data[i];
    }
    return HAL_OK;
}

HAL_StatusTypeDef hal_uart_receive(uint8_t *data, uint16_t len,
                                    uint32_t timeout_ms) {
    if (rx_inject_index + len > rx_inject_count) {
        return HAL_TIMEOUT;
    }
    memcpy(data, &rx_inject[rx_inject_index], len);
    rx_inject_index += len;
    return HAL_OK;
}
