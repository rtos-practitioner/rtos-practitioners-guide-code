/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.12 Lab 17: Designing an Event-Driven Application > Complete Code Listing
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* lab17_main.c -- Event-Driven Application Lab */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>

/* ---- Event definitions ---- */
typedef enum {
    EVT_NONE = 0,
    EVT_TICK_SAMPLE,
    EVT_SENSOR_DATA,
    EVT_PROCESSED_DATA,
    EVT_BUTTON_PRESS,
    EVT_UART_RX,
    EVT_UART_TX_DONE,
    EVT_COMM_CONNECTED,
    EVT_COMM_DISCONNECTED,
    EVT_ERROR,
    EVT_TYPE_COUNT
} EventType_t;

typedef struct {
    EventType_t eType;
    uint32_t    ulTimestamp;
    uint32_t    ulPayload;
} Event_t;

/* ---- Active Object framework ---- */
typedef struct ActiveObject {
    QueueHandle_t xQueue;
    TaskHandle_t  xTask;
    void (*pfInit)(struct ActiveObject *);
    void (*pfDispatch)(struct ActiveObject *, const Event_t *);
} ActiveObject_t;

static void vAOTask(void *pv)
{
    ActiveObject_t *ao = (ActiveObject_t *)pv;
    Event_t evt;
    if (ao->pfInit) ao->pfInit(ao);
    for (;;) {
        if (xQueueReceive(ao->xQueue, &evt,
                          portMAX_DELAY) == pdTRUE) {
            ao->pfDispatch(ao, &evt);
        }
    }
}

static void vAOStart(ActiveObject_t *ao, const char *name,
                     UBaseType_t prio, UBaseType_t qLen,
                     configSTACK_DEPTH_TYPE stack)
{
    ao->xQueue = xQueueCreate(qLen, sizeof(Event_t));
    configASSERT(ao->xQueue);
    xTaskCreate(vAOTask, name, stack, ao, prio, &ao->xTask);
}

static volatile uint32_t ulAODropCount;  /* events lost to full queues */

/* Never blocks: safe from timer callbacks and dispatch functions */
static void vAOPost(ActiveObject_t *ao, const Event_t *e)
{
    if (xQueueSend(ao->xQueue, e, 0) != pdPASS) {
        taskENTER_CRITICAL();
        ulAODropCount++;
        taskEXIT_CRITICAL();
    }
}

/* ---- Pub-Sub Broker ---- */
#define MAX_SUBS  8
typedef struct {
    ActiveObject_t *apSubs[MAX_SUBS];
    uint8_t ucCount;
} SubList_t;

static SubList_t xSubs[EVT_TYPE_COUNT];

static void vSubscribe(EventType_t t, ActiveObject_t *ao)
{
    SubList_t *sl = &xSubs[t];
    configASSERT(sl->ucCount < MAX_SUBS);
    sl->apSubs[sl->ucCount++] = ao;
}

static void vPublish(const Event_t *e)
{
    SubList_t *sl = &xSubs[e->eType];
    for (uint8_t i = 0; i < sl->ucCount; i++) {
        vAOPost(sl->apSubs[i], e);
    }
}

/* ---- Startup synchronization ---- */
#define SENSOR_RDY  (1 << 0)
#define PROC_RDY    (1 << 1)
#define COMM_RDY    (1 << 2)
#define UI_RDY      (1 << 3)
#define ALL_RDY     (SENSOR_RDY | PROC_RDY | COMM_RDY | UI_RDY)

static EventGroupHandle_t xStartupEG;

/* ---- 1. Sensor Manager ---- */
typedef struct {
    ActiveObject_t xBase;
    TimerHandle_t  xSampleTimer;
} SensorMgr_t;

static SensorMgr_t xSensorMgr;

static void vSensorTimerCb(TimerHandle_t t)
{
    Event_t e = { .eType = EVT_TICK_SAMPLE,
                  .ulTimestamp = xTaskGetTickCount() };
    vAOPost(&xSensorMgr.xBase, &e);
}

static void vSensorInit(ActiveObject_t *ao)
{
    SensorMgr_t *s = (SensorMgr_t *)ao;
    s->xSampleTimer = xTimerCreate("Samp", pdMS_TO_TICKS(250),
                                   pdTRUE, NULL, vSensorTimerCb);
    configASSERT(s->xSampleTimer);
    xEventGroupSetBits(xStartupEG, SENSOR_RDY);

    /* The sensor is the data source: hold sampling until every
       active object has initialized */
    EventBits_t bits = xEventGroupWaitBits(xStartupEG, ALL_RDY,
                                           pdFALSE, pdTRUE,
                                           pdMS_TO_TICKS(2000));
    if ((bits & ALL_RDY) != ALL_RDY) {
        /* Degraded start: UI shows the red error LED */
        Event_t err = { .eType = EVT_ERROR,
                        .ulTimestamp = xTaskGetTickCount(),
                        .ulPayload = ALL_RDY & ~bits };
        vPublish(&err);
    }
    xTimerStart(s->xSampleTimer, 0);
}

static void vSensorDispatch(ActiveObject_t *ao, const Event_t *e)
{
    if (e->eType == EVT_TICK_SAMPLE) {
        /* Read ADC (internal temperature sensor on ADC1 CH16) */
        uint32_t raw = 2048; /* Replace with ulReadADC() */
        Event_t pub = { .eType = EVT_SENSOR_DATA,
                        .ulTimestamp = xTaskGetTickCount(),
                        .ulPayload = raw };
        vPublish(&pub);
    }
}

/* ---- 2. Data Processor ---- */
typedef struct {
    ActiveObject_t xBase;
    uint32_t       ulFilterAccum;
    uint8_t        ucSampleCount;
} DataProc_t;

static DataProc_t xDataProc;

static void vDataProcInit(ActiveObject_t *ao)
{
    (void)ao;
    xEventGroupSetBits(xStartupEG, PROC_RDY);
}

static void vDataProcDispatch(ActiveObject_t *ao, const Event_t *e)
{
    DataProc_t *dp = (DataProc_t *)ao;
    if (e->eType == EVT_SENSOR_DATA) {
        dp->ulFilterAccum += e->ulPayload;
        dp->ucSampleCount++;
        if (dp->ucSampleCount >= 4) {
            uint32_t avg = dp->ulFilterAccum / 4;
            dp->ulFilterAccum = 0;
            dp->ucSampleCount = 0;
            Event_t pub = { .eType = EVT_PROCESSED_DATA,
                            .ulTimestamp = xTaskGetTickCount(),
                            .ulPayload = avg };
            vPublish(&pub);
        }
    }
}

/* ---- 3. Comm Manager (with state machine) ---- */
typedef enum {
    COMM_DISCONNECTED, COMM_CONNECTED, COMM_ERROR
} CommState_t;

typedef struct {
    ActiveObject_t xBase;
    CommState_t    eState;
} CommMgr_t;

static CommMgr_t xCommMgr;

static void vCommDispatch(ActiveObject_t *ao, const Event_t *e)
{
    CommMgr_t *cm = (CommMgr_t *)ao;

    switch (cm->eState) {
    case COMM_DISCONNECTED:
        if (e->eType == EVT_UART_RX) {
            /* Received handshake byte */
            cm->eState = COMM_CONNECTED;
            Event_t pub = { .eType = EVT_COMM_CONNECTED,
                            .ulTimestamp = xTaskGetTickCount() };
            vPublish(&pub);
        }
        break;

    case COMM_CONNECTED:
        if (e->eType == EVT_PROCESSED_DATA) {
            /* Transmit processed data over UART */
            uint8_t buf[4];
            memcpy(buf, &e->ulPayload, 4);
            /* usUartTransmit(buf, 4); */
        } else if (e->eType == EVT_ERROR) {
            cm->eState = COMM_ERROR;
        }
        break;

    case COMM_ERROR:
        if (e->eType == EVT_UART_RX) {
            cm->eState = COMM_CONNECTED;
        }
        break;
    }
}

static void vCommInit(ActiveObject_t *ao)
{
    CommMgr_t *cm = (CommMgr_t *)ao;
    cm->eState = COMM_DISCONNECTED;
    xEventGroupSetBits(xStartupEG, COMM_RDY);
}

/* ---- 4. UI Controller ---- */
typedef struct {
    ActiveObject_t xBase;
    bool           bLedOn;
} UiCtrl_t;

static UiCtrl_t xUiCtrl;

static void vUiInit(ActiveObject_t *ao)
{
    UiCtrl_t *ui = (UiCtrl_t *)ao;
    ui->bLedOn = false;
    xEventGroupSetBits(xStartupEG, UI_RDY);
}

static void vUiDispatch(ActiveObject_t *ao, const Event_t *e)
{
    UiCtrl_t *ui = (UiCtrl_t *)ao;

    switch (e->eType) {
    case EVT_PROCESSED_DATA:
        /* Blink green LED (PD12) to show activity */
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        break;

    case EVT_BUTTON_PRESS:
        /* Toggle orange LED (PD13) on button press */
        ui->bLedOn = !ui->bLedOn;
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,
                          ui->bLedOn ? GPIO_PIN_SET
                                     : GPIO_PIN_RESET);
        break;

    case EVT_COMM_CONNECTED:
        /* Blue LED (PD15) on when connected */
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
        break;

    case EVT_COMM_DISCONNECTED:
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
        break;

    case EVT_ERROR:
        /* Red LED (PD14) on for error */
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        break;

    default:
        break;
    }
}

/* ---- Button ISR ---- */
void EXTI0_IRQHandler(void)
{
    BaseType_t xWake = pdFALSE;
    Event_t e = { .eType = EVT_BUTTON_PRESS,
                  .ulTimestamp = xTaskGetTickCountFromISR() };
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    xQueueSendFromISR(xUiCtrl.xBase.xQueue, &e, &xWake);
    portYIELD_FROM_ISR(xWake);
}

/* ---- Startup ---- */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    /* GPIO, UART, ADC init omitted for brevity; enable the EXTI0 IRQ
       only after the UI queue has been created below */

    memset(xSubs, 0, sizeof(xSubs));
    xStartupEG = xEventGroupCreate();
    configASSERT(xStartupEG);

    /* Create active objects */
    xSensorMgr.xBase.pfInit     = vSensorInit;
    xSensorMgr.xBase.pfDispatch = vSensorDispatch;
    vAOStart(&xSensorMgr.xBase, "Sensor", 3, 16, 256);

    xDataProc.xBase.pfInit     = vDataProcInit;
    xDataProc.xBase.pfDispatch = vDataProcDispatch;
    vAOStart(&xDataProc.xBase, "DataProc", 2, 16, 256);

    xCommMgr.xBase.pfInit     = vCommInit;
    xCommMgr.xBase.pfDispatch = vCommDispatch;
    vAOStart(&xCommMgr.xBase, "Comm", 2, 16, 256);

    xUiCtrl.xBase.pfInit     = vUiInit;
    xUiCtrl.xBase.pfDispatch = vUiDispatch;
    vAOStart(&xUiCtrl.xBase, "UI", 1, 16, 256);

    /* Register subscriptions */
    vSubscribe(EVT_SENSOR_DATA,    &xDataProc.xBase);
    vSubscribe(EVT_PROCESSED_DATA, &xCommMgr.xBase);
    vSubscribe(EVT_PROCESSED_DATA, &xUiCtrl.xBase);
    vSubscribe(EVT_COMM_CONNECTED, &xUiCtrl.xBase);
    vSubscribe(EVT_ERROR,          &xUiCtrl.xBase);

    HAL_NVIC_EnableIRQ(EXTI0_IRQn);  /* UI queue now exists */
    vTaskStartScheduler();
    for (;;) {}
}
