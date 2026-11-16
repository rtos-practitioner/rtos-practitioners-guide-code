/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.7 Direct Memory Access (DMA) Integration > UART DMA Receive with Idle Line Detection
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define RX_BUF_SIZE 256
static uint8_t ucDmaRxBuf[RX_BUF_SIZE] __attribute__((section(".sram")));
static QueueHandle_t xRxQueue;

/* Called once during initialization */
void UART_DMA_Init(void)
{
    /* Configure DMA1 Stream 5, Channel 4 for USART2_RX */
    DMA1_Stream5->CR  = (4U << DMA_SxCR_CHSEL_Pos) /* Channel 4      */
                      | DMA_SxCR_MINC               /* Memory incr    */
                      | DMA_SxCR_CIRC               /* Circular mode  */
                      | DMA_SxCR_TCIE;              /* TC interrupt   */
    DMA1_Stream5->NDTR = RX_BUF_SIZE;
    DMA1_Stream5->PAR  = (uint32_t)&USART2->DR;
    DMA1_Stream5->M0AR = (uint32_t)ucDmaRxBuf;
    DMA1_Stream5->CR  |= DMA_SxCR_EN;              /* Enable stream  */
    USART2->CR3       |= USART_CR3_DMAR;           /* UART RX -> DMA */

    /* Enable UART IDLE interrupt */
    USART2->CR1 |= USART_CR1_IDLEIE;

    NVIC_SetPriority(USART2_IRQn, 6);              /* RTOS-safe      */
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(DMA1_Stream5_IRQn, 6);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

/* UART ISR handles idle line detection */
void USART2_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;

    if (USART2->SR & USART_SR_IDLE)
    {
        (void)USART2->DR;                          /* Clear IDLE flag */

        /* Calculate bytes received */
        uint16_t usPos = RX_BUF_SIZE - (uint16_t)DMA1_Stream5->NDTR;

        /* Send position to processing task */
        xQueueSendFromISR(xRxQueue, &usPos, &xWoken);
    }

    portYIELD_FROM_ISR(xWoken);
}
