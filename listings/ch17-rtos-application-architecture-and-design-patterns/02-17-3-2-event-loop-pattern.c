/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.3 Event-Driven Architecture > 17.3.2 Event Loop Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define EVENT_QUEUE_LENGTH  32

static QueueHandle_t xEventQueue;

/* ISR posts hardware events */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    Event_t xEvent = {
        .eType = EVT_BUTTON_PRESSED,
        .ulTimestamp = xTaskGetTickCountFromISR(),
        .ulPayload = 0
    };

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    xQueueSendFromISR(xEventQueue, &xEvent, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Timer callback posts temporal events */
void vSamplingTimerCallback(TimerHandle_t xTimer)
{
    Event_t xEvent = {
        .eType = EVT_TIMER_EXPIRED,
        .ulTimestamp = xTaskGetTickCount(),
        .ulPayload = (uint32_t)pvTimerGetTimerID(xTimer)
    };
    xQueueSend(xEventQueue, &xEvent, 0);
}
