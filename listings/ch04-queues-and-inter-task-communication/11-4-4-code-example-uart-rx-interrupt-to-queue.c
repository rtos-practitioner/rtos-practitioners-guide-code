/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.4 ISR-Safe Queue Operations > Code Example: UART RX Interrupt to Queue
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static QueueHandle_t xUartRxQueue;

/* UART receive interrupt handler */
void USART2_IRQHandler( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if( USART2->SR & USART_SR_RXNE )
    {
        uint8_t ucByte = ( uint8_t )( USART2->DR & 0xFF );

        xQueueSendFromISR( xUartRxQueue, &ucByte,
                           &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/* Deferred processing task: assembles bytes into frames */
void vUartProcessTask( void *pvParameters )
{
    uint8_t ucByte;
    for( ;; )
    {
        if( xQueueReceive( xUartRxQueue, &ucByte,
                            portMAX_DELAY ) == pdPASS )
        {
            vFrameAssembler_AddByte( ucByte );
        }
    }
}
