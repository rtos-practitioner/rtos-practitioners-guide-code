/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.3 Counting Semaphores > 5.3.2 Use Case 2: Resource Pool Management
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define NUM_DMA_CHANNELS  3

static SemaphoreHandle_t xDmaPool;

void vInitDmaPool(void)
{
    /* 3 channels available at startup */
    xDmaPool = xSemaphoreCreateCounting(NUM_DMA_CHANNELS, NUM_DMA_CHANNELS);
    configASSERT(xDmaPool != NULL);
}

DMA_Channel_TypeDef *pxAcquireDmaChannel(TickType_t xTimeout)
{
    if (xSemaphoreTake(xDmaPool, xTimeout) == pdTRUE)
    {
        /* Find and return a free channel from the pool.
         * The semaphore guarantees that at least one is available. */
        return pxFindFreeChannel();
    }
    return NULL;  /* Timeout -- no channel available */
}

void vReleaseDmaChannel(DMA_Channel_TypeDef *pxChannel)
{
    vMarkChannelFree(pxChannel);
    xSemaphoreGive(xDmaPool);
}
