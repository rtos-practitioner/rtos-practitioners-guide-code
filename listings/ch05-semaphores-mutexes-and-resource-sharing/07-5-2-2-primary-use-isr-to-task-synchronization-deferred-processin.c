/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.2 Binary Semaphores > 5.2.2 Primary Use: ISR-to-Task Synchronization (Deferred Processing)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static SemaphoreHandle_t xAdcSemaphore;

void ADC_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Clear the interrupt flag */
    ADC1->SR &= ~ADC_SR_EOC;

    /* Signal the processing task */
    xSemaphoreGiveFromISR(xAdcSemaphore, &xHigherPriorityTaskWoken);

    /* Request context switch if a higher-priority task was woken */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vAdcProcessingTask(void *pvParameters)
{
    uint16_t usRawValue;
    float    fVoltage;

    for (;;)
    {
        /* Block indefinitely until ISR signals conversion complete */
        if (xSemaphoreTake(xAdcSemaphore, portMAX_DELAY) == pdTRUE)
        {
            usRawValue = ADC1->DR;
            fVoltage   = (float)usRawValue * 3.3f / 4096.0f;
            /* Apply digital filter, update control loop, log result... */
        }
    }
}

void vSetupAdc(void)
{
    xAdcSemaphore = xSemaphoreCreateBinary();
    configASSERT(xAdcSemaphore != NULL);

    xTaskCreate(vAdcProcessingTask, "ADC", 256, NULL, 3, NULL);
    /* Configure ADC1 with EOC interrupt enabled... */
}
