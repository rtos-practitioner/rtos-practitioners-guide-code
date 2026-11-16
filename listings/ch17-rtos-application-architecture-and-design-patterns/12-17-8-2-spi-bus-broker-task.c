/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.8 Resource Management Patterns > 17.8.2 SPI Bus Broker Task
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    uint8_t        ucChipSelect;
    const uint8_t *pucTxData;
    uint8_t       *pucRxData;
    size_t         xLength;
    SemaphoreHandle_t xDoneSem;  /* Signaled when complete */
    ErrorCode_t   *peResult;
} SpiBrokerRequest_t;

static QueueHandle_t xSpiBrokerQueue;

void vSpiBrokerTask(void *pvParam)
{
    SpiBrokerRequest_t xReq;
    xSpiBrokerQueue = xQueueCreate(8, sizeof(SpiBrokerRequest_t));
    configASSERT(xSpiBrokerQueue != NULL);

    for (;;) {
        if (xQueueReceive(xSpiBrokerQueue, &xReq,
                          portMAX_DELAY) == pdTRUE) {
            vSpiSelectChip(xReq.ucChipSelect);
            *xReq.peResult = eSpiTransfer(xReq.pucTxData,
                                          xReq.pucRxData,
                                          xReq.xLength);
            vSpiDeselectChip(xReq.ucChipSelect);
            xSemaphoreGive(xReq.xDoneSem);
        }
    }
}

/* Client usage */
ErrorCode_t eSpiBrokerTransact(uint8_t ucCS, const uint8_t *pucTx,
                               uint8_t *pucRx, size_t xLen)
{
    ErrorCode_t eResult;
    SemaphoreHandle_t xDone = xSemaphoreCreateBinary();

    SpiBrokerRequest_t xReq = {
        .ucChipSelect = ucCS,
        .pucTxData    = pucTx,
        .pucRxData    = pucRx,
        .xLength      = xLen,
        .xDoneSem     = xDone,
        .peResult     = &eResult,
    };

    xQueueSend(xSpiBrokerQueue, &xReq, portMAX_DELAY);
    xSemaphoreTake(xDone, portMAX_DELAY);  /* Block until done */
    vSemaphoreDelete(xDone);
    return eResult;
}
