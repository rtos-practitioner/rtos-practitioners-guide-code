/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.12 Lab 3: Multi-Task Scheduling Analysis > Complete Code Listing
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Lab 3: Multi-Task Scheduling Analysis
 * Platform: STM32F407 + FreeRTOS
 * Chapter 3 — Tasks, Priorities, and the Scheduler
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

/* ---------- GPIO Definitions ---------- */
#define FAST_PIN        GPIO_PIN_12
#define MEDIUM_PIN      GPIO_PIN_13
#define SLOW_PIN        GPIO_PIN_14
#define BACKGROUND_PIN  GPIO_PIN_15
#define LED_PORT        GPIOD

/* ---------- DWT Cycle Counter Helpers ---------- */
static inline void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t DWT_GetCycles(void)
{
    return DWT->CYCCNT;
}

/* ---------- Runtime Stats Timer (TIM2 at 1 MHz) ---------- */
static TIM_HandleTypeDef hRunTimeTim;

void vConfigureTimerForRunTimeStats(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    hRunTimeTim.Instance = TIM2;
    hRunTimeTim.Init.Prescaler = ((2 * HAL_RCC_GetPCLK1Freq()) / 1000000) - 1; /* APB1 timer clock = 2 x PCLK1 = 84 MHz -> 1 us tick */
    hRunTimeTim.Init.CounterMode = TIM_COUNTERMODE_UP;
    hRunTimeTim.Init.Period = 0xFFFFFFFF;
    HAL_TIM_Base_Init(&hRunTimeTim);
    HAL_TIM_Base_Start(&hRunTimeTim);
}

uint32_t ulGetRunTimeCounterValue(void)
{
    return __HAL_TIM_GET_COUNTER(&hRunTimeTim);
}

/* ---------- Timing Measurement Structures ---------- */
typedef struct {
    uint32_t lastWakeTime;
    uint32_t maxJitter;       /* in DWT cycles */
    uint32_t iterations;
} TaskStats_t;

/* ---------- Task Functions ---------- */
void vTaskFast(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(10);
    const uint32_t periodCycles = 168000000 / 100;  /* 10 ms in cycles */
    uint32_t expectedCycle = DWT_GetCycles();      /* First release defines the grid */
    uint32_t maxJitter = 0;

    for (;;)
    {
        /* Measure jitter: compare actual wake time to the fixed release grid */
        uint32_t now = DWT_GetCycles();
        int32_t delta = (int32_t)(now - expectedCycle);  /* wrap-safe */
        uint32_t jitter = (delta >= 0) ? (uint32_t)delta : (uint32_t)(-delta);

        if (jitter > maxJitter)
            maxJitter = jitter;

        /* Toggle GPIO for oscilloscope observation */
        HAL_GPIO_TogglePin(LED_PORT, FAST_PIN);

        /* Simulate workload: ~0.5 ms of computation */
        volatile uint32_t count = 0;
        while (count < 14000) count++;

        /* Advance the expected release by exactly one period */
        expectedCycle += periodCycles;

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void vTaskMedium(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(50);

    for (;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, MEDIUM_PIN);

        /* Simulate workload: ~2 ms of computation */
        volatile uint32_t count = 0;
        while (count < 56000) count++;

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void vTaskSlow(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(200);

    for (;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, SLOW_PIN);

        /* Simulate workload: ~5 ms of computation */
        volatile uint32_t count = 0;
        while (count < 140000) count++;

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void vTaskBackground(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(1000);
    static char statsBuffer[512];

    for (;;)
    {
        HAL_GPIO_TogglePin(LED_PORT, BACKGROUND_PIN);

        /* Print runtime statistics via SWO or UART */
        vTaskGetRunTimeStats(statsBuffer);
        /* Send statsBuffer to debug output here */

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

/* ---------- Starvation Scenario ---------- */
/* Uncomment the following task and its creation in main() to observe
 * starvation. This task runs at priority 5 (above all others) and
 * never blocks, consuming 100% of the CPU. */
/*
void vTaskStarve(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        volatile uint32_t i;
        for (i = 0; i < 1000000; i++);
        // NOTE: No vTaskDelay() — this task never yields!
    }
}
*/

/* ---------- GPIO Initialization ---------- */
static void GPIO_Init(void)
{
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin  = FAST_PIN | MEDIUM_PIN | SLOW_PIN | BACKGROUND_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_PORT, &gpio);
}

/* ---------- Main ---------- */
int main(void)
{
    HAL_Init();
    SystemClock_Config();   /* Configure system clock to 168 MHz */
    GPIO_Init();
    DWT_Init();

    /* Create the four periodic tasks */
    xTaskCreate(vTaskFast,       "Fast",   256, NULL, 4, NULL);
    xTaskCreate(vTaskMedium,     "Medium", 256, NULL, 3, NULL);
    xTaskCreate(vTaskSlow,       "Slow",   256, NULL, 2, NULL);
    xTaskCreate(vTaskBackground, "Bkgnd",  512, NULL, 1, NULL);

    /* Uncomment to create the starvation task */
    /* xTaskCreate(vTaskStarve, "Starve", 256, NULL, 5, NULL); */

    vTaskStartScheduler();
    for (;;);
}

/* ---------- FreeRTOS Hook Functions ---------- */
void vApplicationIdleHook(void)
{
    /* Could enter low-power mode here */
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    /* Halt execution — stack overflow detected */
    (void)xTask;
    (void)pcTaskName;
    for (;;);
}
