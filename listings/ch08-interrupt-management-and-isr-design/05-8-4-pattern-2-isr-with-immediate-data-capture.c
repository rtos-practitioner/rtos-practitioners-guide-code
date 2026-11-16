/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.4 ISR Design Patterns > Pattern 2: ISR with Immediate Data Capture
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Queue holds ADC readings; processing task consumes them */
static QueueHandle_t xAdcQueue;

void ADC_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint16_t   usReading;

    if (ADC1->SR & ADC_SR_EOC)
    {
        usReading = (uint16_t)ADC1->DR;          /* Read clears EOC   */

        xQueueSendFromISR(xAdcQueue, &usReading,
                          &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
