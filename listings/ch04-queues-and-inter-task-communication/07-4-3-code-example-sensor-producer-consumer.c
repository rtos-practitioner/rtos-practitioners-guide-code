/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.3 Sending and Receiving > Code Example: Sensor Producer-Consumer
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    uint16_t usSensorID;
    float    fValue;
    TickType_t xTimestamp;
} SensorReading_t;

static QueueHandle_t xSensorQueue;

/* Producer: reads ADC and enqueues a reading every 100 ms */
void vSensorTask( void *pvParameters )
{
    SensorReading_t xReading;
    xReading.usSensorID = ( uint16_t )( uint32_t ) pvParameters;

    for( ;; )
    {
        xReading.fValue     = fReadADC( xReading.usSensorID );
        xReading.xTimestamp = xTaskGetTickCount();

        if( xQueueSend( xSensorQueue, &xReading,
                         pdMS_TO_TICKS( 10 ) ) != pdPASS )
        {
            /* Queue full --- data lost. Increment diagnostic counter. */
            ulSensorDropCount++;
        }

        vTaskDelay( pdMS_TO_TICKS( 100 ) );
    }
}

/* Consumer: blocks until a reading arrives, then processes it */
void vProcessingTask( void *pvParameters )
{
    SensorReading_t xReceived;

    for( ;; )
    {
        if( xQueueReceive( xSensorQueue, &xReceived,
                            portMAX_DELAY ) == pdPASS )
        {
            vApplyFilter( xReceived.usSensorID, xReceived.fValue );
            vLogReading( &xReceived );
        }
    }
}

/* In main(), before starting the scheduler: */
xSensorQueue = xQueueCreate( 16, sizeof( SensorReading_t ) );
configASSERT( xSensorQueue != NULL );
xTaskCreate( vSensorTask, "Sens0", 256, ( void * ) 0, 2, NULL );
xTaskCreate( vSensorTask, "Sens1", 256, ( void * ) 1, 2, NULL );
xTaskCreate( vProcessingTask, "Proc", 512, NULL, 3, NULL );
