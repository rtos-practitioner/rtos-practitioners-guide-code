/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.7 Task Notifications as Lightweight IPC > Lightweight Binary/Counting Semaphore
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* ISR signals the task */
void EXTI0_IRQHandler( void )
{
    BaseType_t xWoken = pdFALSE;
    vTaskNotifyGiveFromISR( xProcessingTaskHandle, &xWoken );
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );
    portYIELD_FROM_ISR( xWoken );
}

/* Task waits for the signal */
void vProcessingTask( void *pvParameters )
{
    for( ;; )
    {
        ulTaskNotifyTake( pdTRUE,       /* Clear count on exit */
                          portMAX_DELAY );
        vProcessExternalEvent();
    }
}
