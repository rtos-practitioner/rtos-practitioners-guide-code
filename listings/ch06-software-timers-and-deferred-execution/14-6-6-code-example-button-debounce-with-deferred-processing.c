/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 6: Software Timers and Deferred Execution
 * 6.6 Deferred Interrupt Processing Pattern > Code Example: Button Debounce with Deferred Processing
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include "FreeRTOS.h"
#include "timers.h"
#include "bsp.h"

static TimerHandle_t xDebounceTimer;
static volatile BaseType_t xButtonProcessed = pdTRUE;

static void vButtonAction(void *pvParameter1, uint32_t ulParameter2);

/* Called by daemon task after debounce delay */
static void vDebounceTimerCallback(TimerHandle_t xTimer)
{
    (void)xTimer;

    /* Read the stable button state after debounce period */
    if (BSP_Button_GetState(BUTTON_USER) == BUTTON_PRESSED) {
        /* Confirmed press -- pend the processing function */
        xTimerPendFunctionCall(vButtonAction, NULL, 0, 0);
    }
    xButtonProcessed = pdTRUE;
}

/* The actual button processing, runs in daemon task context */
static void vButtonAction(void *pvParam1, uint32_t ulParam2)
{
    (void)pvParam1;
    (void)ulParam2;

    BSP_LED_Toggle(LED_BLUE);
    /* Could also: send message to a queue, update state machine, etc. */
}

/* GPIO EXTI ISR for the user button */
void EXTI0_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    if (xButtonProcessed == pdTRUE) {
        xButtonProcessed = pdFALSE;
        /* Start 50 ms one-shot debounce timer */
        xTimerResetFromISR(xDebounceTimer, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vDebounceInit(void)
{
    xDebounceTimer = xTimerCreate("Dbnc",
                                  pdMS_TO_TICKS(50),
                                  pdFALSE,       /* One-shot */
                                  NULL,
                                  vDebounceTimerCallback);
}
