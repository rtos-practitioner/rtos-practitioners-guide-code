/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 15: Common RTOS Pitfalls and Anti-Patterns
 * 15.13 Pitfall: Global State and Hidden Coupling > Code Example
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    float fTemperature;
    float fPressure;
} SensorData_t;

static QueueHandle_t xSensorQueue;

void vSystemInit(void)
{
    xSensorQueue = xQueueCreate(5, sizeof(SensorData_t));
    configASSERT(xSensorQueue != NULL);
}

void vSensorTask(void *pvParam)
{
    SensorData_t xData;
    for (;;) {
        xData.fTemperature = fReadTemperatureSensor();
        xData.fPressure = fReadPressureSensor();
        /* Atomic delivery of the complete data structure */
        xQueueSend(xSensorQueue, &xData, pdMS_TO_TICKS(10));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void vDisplayTask(void *pvParam)
{
    SensorData_t xReceived;
    for (;;) {
        /* Blocks until data is available — no polling, no globals */
        if (xQueueReceive(xSensorQueue, &xReceived,
                          portMAX_DELAY) == pdPASS) {
            vUpdateDisplay(xReceived.fTemperature, xReceived.fPressure);
        }
    }
}
