/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.3 Event-Driven Architecture > 17.3.3 Centralized Event Dispatcher
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef void (*EventHandler_t)(const Event_t *pxEvent);

typedef struct {
    EventType_t    eType;
    EventHandler_t pfHandler;
} EventDispatchEntry_t;

static const EventDispatchEntry_t xDispatchTable[] = {
    { EVT_BUTTON_PRESSED,    vHandleButtonPress    },
    { EVT_BUTTON_RELEASED,   vHandleButtonRelease  },
    { EVT_SENSOR_DATA_READY, vHandleSensorData     },
    { EVT_TIMER_EXPIRED,     vHandleTimerExpiry    },
    { EVT_UART_RX_COMPLETE,  vHandleUartRx         },
    { EVT_ERROR_DETECTED,    vHandleError          },
};

#define DISPATCH_TABLE_SIZE \
    (sizeof(xDispatchTable) / sizeof(xDispatchTable[0]))

/* Call from main() before enabling EXTI0 or starting the scheduler,
   so no producer can ever post to a NULL queue handle. */
void vEventDispatcherInit(void)
{
    xEventQueue = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(Event_t));
    configASSERT(xEventQueue != NULL);
}

void vEventDispatcherTask(void *pvParam)
{
    Event_t xEvent;

    for (;;) {
        if (xQueueReceive(xEventQueue, &xEvent, portMAX_DELAY) == pdTRUE) {
            for (size_t i = 0; i < DISPATCH_TABLE_SIZE; i++) {
                if (xDispatchTable[i].eType == xEvent.eType) {
                    xDispatchTable[i].pfHandler(&xEvent);
                    break;
                }
            }
        }
    }
}
