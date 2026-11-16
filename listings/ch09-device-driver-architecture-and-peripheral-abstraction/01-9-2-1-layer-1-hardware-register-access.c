/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.2 Four-Layer Driver Model > 9.2.1 Layer 1: Hardware Register Access
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* From CMSIS device header (read-only reference) */
typedef struct {
    __IO uint32_t SR;    /* Status register,        offset: 0x00 */
    __IO uint32_t DR;    /* Data register,           offset: 0x04 */
    __IO uint32_t BRR;   /* Baud rate register,      offset: 0x08 */
    __IO uint32_t CR1;   /* Control register 1,      offset: 0x0C */
    __IO uint32_t CR2;   /* Control register 2,      offset: 0x10 */
    __IO uint32_t CR3;   /* Control register 3,      offset: 0x14 */
    __IO uint32_t GTPR;  /* Guard time/prescaler,    offset: 0x18 */
} USART_TypeDef;
