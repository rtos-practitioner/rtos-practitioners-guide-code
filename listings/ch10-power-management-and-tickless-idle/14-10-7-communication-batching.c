/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.7 Design Patterns for Low Power > Communication Batching
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define BATCH_SIZE  30
static SensorReading_t xBatch[BATCH_SIZE];
static uint32_t ulBatchIndex = 0;

void vSensorTask(void *pv) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        xBatch[ulBatchIndex++] = ReadSensor();
        if (ulBatchIndex >= BATCH_SIZE) {
            TransmitBatch(xBatch, BATCH_SIZE);
            ulBatchIndex = 0;
        }
    }
}
