/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.6 Fragmentation Analysis and Prevention > Prevention Strategies
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
#define POOL_BLOCK_SIZE   64
#define POOL_BLOCK_COUNT  32

static uint8_t        xPoolStorage[POOL_BLOCK_COUNT][POOL_BLOCK_SIZE];
static QueueHandle_t  xFreeList;

void vPoolInit(void)
{
    xFreeList = xQueueCreate(POOL_BLOCK_COUNT, sizeof(void *));
    for (int i = 0; i < POOL_BLOCK_COUNT; i++)
    {
        void *pBlock = &xPoolStorage[i][0];
        xQueueSend(xFreeList, &pBlock, 0);
    }
}

void *pvPoolAlloc(TickType_t xTimeout)
{
    void *pBlock = NULL;
    xQueueReceive(xFreeList, &pBlock, xTimeout);
    return pBlock;  /* NULL if timeout expired */
}

void vPoolFree(void *pBlock)
{
    xQueueSend(xFreeList, &pBlock, 0);
}
