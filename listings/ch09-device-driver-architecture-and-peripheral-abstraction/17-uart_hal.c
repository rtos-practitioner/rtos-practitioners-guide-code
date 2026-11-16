/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.7 The Fake HAL Pattern for Testing > 9.7.3 Fake HAL Implementation
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* hal/fake/uart_hal.c -- Fake HAL with loopback */
#include "uart_hal.h"
#include "uart_rtos.h"   /* uart_rtos_rx_isr(), _tx_isr(), _tc_isr() */
#include <string.h>

/* hal/fake/stm32f4xx_hal.h routes the interrupt-enable macros here:
 *   #define __HAL_UART_ENABLE_IT(h, it)   fake_uart_enable_it(it)
 *   #define __HAL_UART_DISABLE_IT(h, it)  fake_uart_disable_it(it)
 */

#define FAKE_BUF_SIZE 1024

static uint8_t  fake_tx_buf[FAKE_BUF_SIZE];
static uint16_t fake_tx_head;
static uint8_t  fake_rx_byte;      /* "DR" value seen by the RX ISR */
static int      fake_txeie;
static int      loopback_enabled = 0;

/* Simulated UART handle (matches real HAL structure layout) */
UART_HandleTypeDef huart2;

int uart_hal_init(uint32_t baudrate)
{
    fake_tx_head = 0;
    fake_txeie   = 0;
    memset(&huart2, 0, sizeof(huart2));
    (void)baudrate;  /* Baud rate is irrelevant in simulation */
    return 0;
}

uint8_t uart_hal_read_byte(void)
{
    return fake_rx_byte;
}

/* Every byte the driver's TX ISR "writes to DR" is recorded here */
void uart_hal_write_byte(uint8_t byte)
{
    if (fake_tx_head < FAKE_BUF_SIZE) {
        fake_tx_buf[fake_tx_head++] = byte;
    }
    if (loopback_enabled) {
        fake_rx_byte = byte;
        uart_rtos_rx_isr();
    }
}

/* Simulated interrupts: TXE fires until the driver disables it */
void fake_uart_enable_it(uint32_t it)
{
    if (it == UART_IT_TXE) {
        fake_txeie = 1;
        while (fake_txeie) {
            uart_rtos_tx_isr();
        }
    } else if (it == UART_IT_TC) {
        uart_rtos_tc_isr();         /* Shift register is always idle */
    }
}

void fake_uart_disable_it(uint32_t it)
{
    if (it == UART_IT_TXE) {
        fake_txeie = 0;
    }
}

/* Test harness: inject bytes through the driver's RX ISR path */
void fake_uart_inject_rx(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        fake_rx_byte = data[i];
        uart_rtos_rx_isr();
    }
}

/* Test harness: read what was transmitted */
uint16_t fake_uart_get_tx(uint8_t *buf, uint16_t max_len)
{
    uint16_t count = (fake_tx_head < max_len) ? fake_tx_head : max_len;
    memcpy(buf, fake_tx_buf, count);
    return count;
}

/* Test harness: enable loopback (TX feeds back to RX) */
void fake_uart_set_loopback(int enable)
{
    loopback_enabled = enable;
}
