/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.3 UART Driver Case Study > 9.3.1 Layer 1: Register-Level Access
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* uart_hw.h -- Register-level constants and helpers */
#ifndef UART_HW_H
#define UART_HW_H

#include "stm32f407xx.h"

/* Key USART registers:
 * SR  (Status):  TXE (bit 7), RXNE (bit 5), TC (bit 6), ORE (bit 3)
 * DR  (Data):    bits [8:0] hold TX/RX byte
 * BRR (Baud):    mantissa [15:4], fraction [3:0]
 * CR1 (Control): UE (bit 13), TE (bit 3), RE (bit 2),
 *                TXEIE (bit 7), TCIE (bit 6), RXNEIE (bit 5)
 */

#define UART_SR_TXE    (1U << 7)
#define UART_SR_RXNE   (1U << 5)
#define UART_SR_TC     (1U << 6)
#define UART_CR1_UE    (1U << 13)
#define UART_CR1_TE    (1U << 3)
#define UART_CR1_RE    (1U << 2)
#define UART_CR1_TXEIE (1U << 7)
#define UART_CR1_TCIE  (1U << 6)
#define UART_CR1_RXNEIE (1U << 5)

static inline void uart_hw_send_byte(USART_TypeDef *periph, uint8_t byte)
{
    while (!(periph->SR & UART_SR_TXE))
        ;  /* Wait for TX empty -- polling, used only in early init */
    periph->DR = byte;
}

static inline uint8_t uart_hw_recv_byte(USART_TypeDef *periph)
{
    while (!(periph->SR & UART_SR_RXNE))
        ;
    return (uint8_t)(periph->DR & 0xFF);
}

#endif /* UART_HW_H */
