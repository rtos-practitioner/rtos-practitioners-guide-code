/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.16 Lab 15: Pitfall Detection Workshop > Exercise 1: Identify the Pitfall
 * Verification: anti-pattern: fails exactly as the book describes
 * SPDX-License-Identifier: MIT
 */
void DMA1_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
    printf("DMA transfer complete\n");
    xSemaphoreTake(xResourceMutex, portMAX_DELAY);
    ulTransferCount++;
    xSemaphoreGive(xResourceMutex);
}
