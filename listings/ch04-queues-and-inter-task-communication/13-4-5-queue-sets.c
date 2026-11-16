/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.5 Queue Sets
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
for( ;; )
{
    QueueSetMemberHandle_t xActiveMember;
    xActiveMember = xQueueSelectFromSet( xSet, portMAX_DELAY );

    if( xActiveMember == xUartQueue )
    {
        xQueueReceive( xUartQueue, &xUartCmd, 0 );
        vHandleUartCommand( &xUartCmd );
    }
    else if( xActiveMember == xAdcQueue )
    {
        xQueueReceive( xAdcQueue, &xAdcReading, 0 );
        vHandleAdcReading( &xAdcReading );
    }
    else if( xActiveMember == xNetQueue )
    {
        xQueueReceive( xNetQueue, &xNetStatus, 0 );
        vHandleNetStatus( &xNetStatus );
    }
}
