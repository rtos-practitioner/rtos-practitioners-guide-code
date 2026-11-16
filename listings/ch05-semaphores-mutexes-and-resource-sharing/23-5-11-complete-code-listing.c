/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.11 Lab 5: Priority Inversion Demonstration and Cure > Complete Code Listing
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx.h"

/* ---- Configuration ---- */
/* Change this define to switch between semaphore and mutex */
#define USE_MUTEX  0   /* Set to 1 for mutex (priority inheritance) */

static SemaphoreHandle_t xSharedLock;

/* DWT cycle counter for precise timing */
static inline void vDwtInit(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t ulDwtGetCycles(void)
{
    return DWT->CYCCNT;
}

/* GPIO setup: PD12, PD13, PD14 as push-pull outputs */
static void vGpioInit(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER &= ~((3U << 24) | (3U << 26) | (3U << 28));
    GPIOD->MODER |=  ((1U << 24) | (1U << 26) | (1U << 28));
    GPIOD->OTYPER &= ~((1U << 12) | (1U << 13) | (1U << 14));
}

/* Busy-wait for approximately usDelay microseconds */
static void vBusyWaitUs(uint32_t usDelay)
{
    uint32_t ulStart = ulDwtGetCycles();
    uint32_t ulTarget = usDelay * (SystemCoreClock / 1000000U);
    while ((ulDwtGetCycles() - ulStart) < ulTarget) { }
}

/* --- High-Priority Task (priority 4) --- */
void vTaskHigh(void *pv)
{
    volatile uint32_t ulBlockStart, ulBlockEnd;
    volatile uint32_t ulWorstCaseBlock = 0;

    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(200));

        GPIOD->BSRR = (1U << 12);           /* PD12 HIGH: H wants lock */

        ulBlockStart = ulDwtGetCycles();
        xSemaphoreTake(xSharedLock, portMAX_DELAY);
        ulBlockEnd = ulDwtGetCycles();

        uint32_t ulBlockTime = ulBlockEnd - ulBlockStart;
        if (ulBlockTime > ulWorstCaseBlock)
            ulWorstCaseBlock = ulBlockTime;

        /* Hold the lock for 1 ms (simulated critical section) */
        vBusyWaitUs(1000);

        xSemaphoreGive(xSharedLock);
        GPIOD->BSRR = (1U << (12 + 16));    /* PD12 LOW: H done */
    }
}

/* --- Medium-Priority Task (priority 3) --- */
void vTaskMedium(void *pv)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(50));

        GPIOD->BSRR = (1U << 13);           /* PD13 HIGH: M running */

        /* CPU-intensive work, no lock needed -- 20 ms */
        vBusyWaitUs(20000);

        GPIOD->BSRR = (1U << (13 + 16));    /* PD13 LOW: M idle */
    }
}

/* --- Low-Priority Task (priority 1) --- */
void vTaskLow(void *pv)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(300));

        xSemaphoreTake(xSharedLock, portMAX_DELAY);
        GPIOD->BSRR = (1U << 14);           /* PD14 HIGH: L in critical */

        /* Hold the lock for 5 ms (longer critical section) */
        vBusyWaitUs(5000);

        GPIOD->BSRR = (1U << (14 + 16));    /* PD14 LOW */
        xSemaphoreGive(xSharedLock);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();   /* 168 MHz, see BSP */
    vGpioInit();
    vDwtInit();

#if USE_MUTEX
    xSharedLock = xSemaphoreCreateMutex();     /* Priority inheritance ON */
#else
    xSharedLock = xSemaphoreCreateBinary();    /* No priority inheritance */
    xSemaphoreGive(xSharedLock);               /* Start in "available" state */
#endif
    configASSERT(xSharedLock != NULL);

    xTaskCreate(vTaskHigh,   "H", 256, NULL, 4, NULL);
    xTaskCreate(vTaskMedium, "M", 256, NULL, 3, NULL);
    xTaskCreate(vTaskLow,    "L", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    for (;;) { }
}
