/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.4 Active Object Pattern > 17.4.3 Code Example: LED Controller Active Object
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* --- Active Object infrastructure --- */
typedef struct ActiveObject {
    QueueHandle_t  xQueue;
    TaskHandle_t   xTask;
    void         (*pfInit)(struct ActiveObject *pxAO);
    void         (*pfDispatch)(struct ActiveObject *pxAO,
                               const Event_t *pxEvent);
    uint32_t       ulPostFailures;  /* events lost to a full queue */
} ActiveObject_t;

static void vActiveObjectTask(void *pvParam)
{
    ActiveObject_t *pxAO = (ActiveObject_t *)pvParam;
    Event_t xEvent;

    if (pxAO->pfInit != NULL) {
        pxAO->pfInit(pxAO);
    }

    for (;;) {
        if (xQueueReceive(pxAO->xQueue, &xEvent,
                          portMAX_DELAY) == pdTRUE) {
            pxAO->pfDispatch(pxAO, &xEvent);
        }
    }
}

void vActiveObjectStart(ActiveObject_t *pxAO,
                        const char *pcName,
                        UBaseType_t uxPriority,
                        UBaseType_t uxQueueLen,
                        configSTACK_DEPTH_TYPE uxStackDepth)
{
    pxAO->xQueue = xQueueCreate(uxQueueLen, sizeof(Event_t));
    configASSERT(pxAO->xQueue != NULL);

    BaseType_t xResult = xTaskCreate(
        vActiveObjectTask, pcName, uxStackDepth,
        pxAO, uxPriority, &pxAO->xTask);
    configASSERT(xResult == pdPASS);
}

/* Use xTicksToWait = 0 from timer callbacks and dispatch functions,
   which must never block. */
BaseType_t xActiveObjectPost(ActiveObject_t *pxAO,
                             const Event_t *pxEvent,
                             TickType_t xTicksToWait)
{
    BaseType_t xResult = xQueueSend(pxAO->xQueue, pxEvent,
                                    xTicksToWait);
    if (xResult != pdPASS) {
        taskENTER_CRITICAL();
        pxAO->ulPostFailures++;
        taskEXIT_CRITICAL();
    }
    return xResult;
}

/* --- LED Controller (concrete active object) --- */
typedef enum { LED_STATE_OFF, LED_STATE_ON, LED_STATE_BLINK } LedState_t;

enum { EVT_LED_ON = EVT_ERROR_DETECTED + 1, EVT_LED_OFF, EVT_LED_BLINK };

typedef struct {
    ActiveObject_t xBase;
    LedState_t     eState;
    TimerHandle_t  xBlinkTimer;
} LedController_t;

static LedController_t xLedCtrl;

static void vLedBlinkTimerCb(TimerHandle_t xTimer)
{
    Event_t xEvt = { .eType = EVT_TIMER_EXPIRED };
    /* Timer callbacks run in the timer service task: never block */
    (void)xActiveObjectPost(&xLedCtrl.xBase, &xEvt, 0);
}

static void vLedInit(ActiveObject_t *pxAO)
{
    LedController_t *pxLed = (LedController_t *)pxAO;
    pxLed->eState = LED_STATE_OFF;
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
    pxLed->xBlinkTimer = xTimerCreate("LedBlink",
        pdMS_TO_TICKS(500), pdTRUE, NULL, vLedBlinkTimerCb);
}

static void vLedDispatch(ActiveObject_t *pxAO,
                         const Event_t *pxEvent)
{
    LedController_t *pxLed = (LedController_t *)pxAO;

    switch (pxLed->eState) {
    case LED_STATE_OFF:
        if (pxEvent->eType == EVT_LED_ON) {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
            pxLed->eState = LED_STATE_ON;
        } else if (pxEvent->eType == EVT_LED_BLINK) {
            xTimerStart(pxLed->xBlinkTimer, 0);
            pxLed->eState = LED_STATE_BLINK;
        }
        break;

    case LED_STATE_ON:
        if (pxEvent->eType == EVT_LED_OFF) {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
            pxLed->eState = LED_STATE_OFF;
        } else if (pxEvent->eType == EVT_LED_BLINK) {
            xTimerStart(pxLed->xBlinkTimer, 0);
            pxLed->eState = LED_STATE_BLINK;
        }
        break;

    case LED_STATE_BLINK:
        if (pxEvent->eType == EVT_TIMER_EXPIRED) {
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        } else if (pxEvent->eType == EVT_LED_OFF) {
            xTimerStop(pxLed->xBlinkTimer, 0);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
            pxLed->eState = LED_STATE_OFF;
        } else if (pxEvent->eType == EVT_LED_ON) {
            xTimerStop(pxLed->xBlinkTimer, 0);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
            pxLed->eState = LED_STATE_ON;
        }
        break;
    }
}

/* Initialization */
void vLedControllerCreate(void)
{
    xLedCtrl.xBase.pfInit     = vLedInit;
    xLedCtrl.xBase.pfDispatch = vLedDispatch;
    vActiveObjectStart(&xLedCtrl.xBase, "LED", 2, 16, 256);
}
