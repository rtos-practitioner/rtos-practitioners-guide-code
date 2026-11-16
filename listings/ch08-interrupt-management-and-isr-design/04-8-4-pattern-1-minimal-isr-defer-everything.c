/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.4 ISR Design Patterns > Pattern 1: Minimal ISR — Defer Everything
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Binary semaphore given from ISR, taken by handler task */
static SemaphoreHandle_t xDmaSemaphore;

void DMA1_Stream6_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (DMA1->HISR & DMA_HISR_TCIF6)           /* Transfer complete? */
    {
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;        /* Clear flag        */

        xSemaphoreGiveFromISR(xDmaSemaphore,
                              &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
