/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.11 Lab 4: Multi-Channel Data Pipeline > Aggregator Task (Consumer)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vAggregatorTask( void *pvParameters )
{
    SensorCommand_t xCmd;
    uint32_t ulCounts[ 3 ] = { 0, 0, 0 };
    float    fSums[ 3 ]    = { 0.0f, 0.0f, 0.0f };
    const char *pcNames[]  = { "TEMP", "PRES", "VIBR" };
    UBaseType_t uxHighWater = 0;

    for( ;; )
    {
        if( xQueueReceive( xCommandQueue, &xCmd,
                            portMAX_DELAY ) == pdPASS )
        {
            /* +1 counts the item just removed */
            UBaseType_t uxNow = uxQueueMessagesWaiting( xCommandQueue ) + 1;
            if( uxNow > uxHighWater )
            {
                uxHighWater = uxNow;
            }

            uint8_t ucID = xCmd.ucSensorID;
            if( ucID < 3 )
            {
                ulCounts[ ucID ]++;
                fSums[ ucID ] += xCmd.fValue;
            }

            /* Print summary every 100 messages */
            uint32_t ulTotal = ulCounts[0] + ulCounts[1] + ulCounts[2];
            if( ( ulTotal % 100 ) == 0 )
            {
                printf( "\r\n--- Aggregator Report (total=%lu) ---\r\n",
                        ( unsigned long ) ulTotal );
                for( int i = 0; i < 3; i++ )
                {
                    if( ulCounts[i] > 0 )
                    {
                        printf( "  %s: n=%lu, avg=%.2f\r\n",
                                pcNames[i],
                                ( unsigned long ) ulCounts[i],
                                ( double )( fSums[i] / ulCounts[i] ) );
                    }
                }
                /* Queue diagnostic */
                printf( "  Queue high-water: %lu / %d\r\n",
                        ( unsigned long ) uxHighWater,
                        QUEUE_DEPTH );
                printf( "  Task enqueue fails: %lu\r\n",
                        ( unsigned long ) ulTaskEnqueueFail );
                printf( "  ISR enqueue fails:  %lu\r\n",
                        ( unsigned long ) ulIsrEnqueueFail );
            }
        }
    }
}
