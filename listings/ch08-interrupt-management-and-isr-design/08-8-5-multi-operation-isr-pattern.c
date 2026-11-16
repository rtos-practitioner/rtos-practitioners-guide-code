/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.5 The FromISR API Family > Multi-Operation ISR Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void USART2_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;
    uint32_t   ulStatus = USART2->SR;

    if (ulStatus & USART_SR_RXNE)
    {
        uint8_t ucByte = (uint8_t)USART2->DR;
        xStreamBufferSendFromISR(xRxStream, &ucByte, 1, &xWoken);
    }

    if (ulStatus & USART_SR_IDLE)
    {
        (void)USART2->DR;                        /* Clear IDLE flag   */
        xTaskNotifyFromISR(xUartTaskHandle, UART_IDLE_EVENT,
                           eSetBits, &xWoken);
    }

    portYIELD_FROM_ISR(xWoken);                   /* Single yield      */
}
