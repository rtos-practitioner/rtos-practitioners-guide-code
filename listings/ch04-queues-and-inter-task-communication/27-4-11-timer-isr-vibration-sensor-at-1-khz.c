/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.11 Lab 4: Multi-Channel Data Pipeline > Timer ISR (Vibration Sensor at 1 kHz)
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
static void prvVibrationTimerCallback( TimerHandle_t xTimer )
{
    /* Note: software timer callbacks run in the timer-service task
       context, not ISR context. For a true ISR source, use a hardware
       timer interrupt. This example uses the software timer for
       portability. For ISR demonstration, see the hardware variant below. */
    SensorCommand_t xCmd;
    xCmd.ucSensorID = SENSOR_VIBRATION;
    xCmd.fValue     = 0.5f + ( float )( xTaskGetTickCount() % 20 ) * 0.01f;
    xCmd.xTimestamp = xTaskGetTickCount();

    if( xQueueSend( xCommandQueue, &xCmd, 0 ) != pdPASS )
    {
        ulTaskEnqueueFail++;
    }
}

/* Hardware timer ISR variant (STM32F407 TIM2) */
void TIM2_IRQHandler( void )
{
    BaseType_t xWoken = pdFALSE;

    if( TIM2->SR & TIM_SR_UIF )
    {
        TIM2->SR = ~TIM_SR_UIF;   /* Clear interrupt flag (rc_w0: no read-modify-write) */

        SensorCommand_t xCmd;
        xCmd.ucSensorID = SENSOR_VIBRATION;
        xCmd.fValue     = 0.5f;   /* Replace with actual accelerometer read */
        xCmd.xTimestamp = xTaskGetTickCountFromISR();

        if( xQueueSendFromISR( xCommandQueue, &xCmd,
                                &xWoken ) != pdPASS )
        {
            ulIsrEnqueueFail++;
        }
    }

    portYIELD_FROM_ISR( xWoken );
}
