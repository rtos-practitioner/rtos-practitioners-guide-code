/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.2 Four-Layer Driver Model > 9.2.2 Layer 2: Vendor HAL
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* HAL-level UART transmit (interrupt mode) */
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart,
                                        const uint8_t *pData,
                                        uint16_t Size);

/* LL-level direct register write */
LL_USART_TransmitData8(USART2, byte);
