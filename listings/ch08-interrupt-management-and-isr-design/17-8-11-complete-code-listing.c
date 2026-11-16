/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 8: Interrupt Management and ISR Design
 * 8.11 Lab 8: Interrupt Latency Measurement and ISR Optimization > Complete Code Listing
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* ===================================================================
 * Lab 8: Interrupt Latency Measurement and ISR Optimization
 * Target: STM32F407 Discovery, FreeRTOS, 168 MHz
 * ================================================================ */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "stream_buffer.h"
#include <string.h>

/* Provided by BSP: uart_printf, HAL_Init, SystemClock_Config, etc. */
extern void uart_printf(const char *fmt, ...);

/* ---------- DWT Cycle Counter Setup ---------- */
static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t DWT_GetCycles(void) { return DWT->CYCCNT; }

/* ---------- Latency measurement variables ---------- */
static volatile uint32_t ulIsrEntryTimestamp;
static volatile uint32_t ulIsrExitTimestamp;
static volatile uint32_t ulLatencyCycles;
static SemaphoreHandle_t xButtonSemaphore;

/* TIM5 free-runs at 84 MHz (APB1 timer clock); CH1 captures the PA0 edge */
static void TIM5_Capture_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    GPIOA->MODER  = (GPIOA->MODER & ~GPIO_MODER_MODER0) | GPIO_MODER_MODER0_1;
    GPIOA->AFR[0] = (GPIOA->AFR[0] & ~0xFU) | 2U;      /* AF2 = TIM5_CH1 */

    TIM5->PSC   = 0;
    TIM5->ARR   = 0xFFFFFFFFU;                          /* 32-bit timer   */
    TIM5->CCMR1 = TIM_CCMR1_CC1S_0;                     /* CC1 = TI1 input */
    TIM5->CCER  = TIM_CCER_CC1P | TIM_CCER_CC1E;        /* Falling edge   */
    TIM5->EGR   = TIM_EGR_UG;
    TIM5->CR1   = TIM_CR1_CEN;
}

/* ---------- Part A: EXTI0 ISR (PA0 button) ---------- */
void EXTI0_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;

    uint32_t ulNow = TIM5->CNT;             /* Read as early as possible */
    ulIsrEntryTimestamp = DWT_GetCycles();
    GPIOC->BSRR = GPIO_BSRR_BS1;           /* PC1 HIGH for scope   */

    if (EXTI->PR & EXTI_PR_PR0)
    {
        EXTI->PR = EXTI_PR_PR0;             /* Clear pending flag   */

        /* Edge time captured by TIM5 hardware; 1 tick = 2 CPU cycles */
        ulLatencyCycles = (ulNow - TIM5->CCR1) * 2U;

        xSemaphoreGiveFromISR(xButtonSemaphore, &xWoken);
    }

    GPIOC->BSRR = GPIO_BSRR_BR1;           /* PC1 LOW              */
    ulIsrExitTimestamp = DWT_GetCycles();

    portYIELD_FROM_ISR(xWoken);
}

/* ---------- Part B: ADC queue for deferred processing ---------- */
#define ADC_QUEUE_LEN  64
static QueueHandle_t xAdcQueue;
static volatile uint32_t ulAdcIsrCount;

void ADC_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;

    if (ADC1->SR & ADC_SR_EOC)
    {
        uint16_t usReading = (uint16_t)ADC1->DR;
        ulAdcIsrCount++;
        xQueueSendFromISR(xAdcQueue, &usReading, &xWoken);
    }

    portYIELD_FROM_ISR(xWoken);
}

/* ---------- Part C: Nesting timer ISRs ---------- */
/* TIM2 at priority 10, TIM3 at priority 8, TIM4 at priority 6.
 * Only TIM2 runs periodically; each ISR forces an update event on the
 * next-higher-priority timer so that ISR preempts it (nesting). */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR = ~TIM_SR_UIF;
        GPIOC->BSRR = GPIO_BSRR_BS2;       /* PC2 HIGH             */
        TIM3->EGR = TIM_EGR_UG;            /* TIM3 IRQ preempts us */
        volatile uint32_t i;
        for (i = 0; i < 84; i++) { __NOP(); }  /* Short busy wait  */
        GPIOC->BSRR = GPIO_BSRR_BR2;       /* PC2 LOW              */
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR = ~TIM_SR_UIF;
        GPIOC->BSRR = GPIO_BSRR_BS3;       /* PC3 HIGH             */
        TIM4->EGR = TIM_EGR_UG;            /* TIM4 IRQ preempts us */
        volatile uint32_t i;
        for (i = 0; i < 84; i++) { __NOP(); }
        GPIOC->BSRR = GPIO_BSRR_BR3;       /* PC3 LOW              */
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF)
    {
        TIM4->SR = ~TIM_SR_UIF;
        GPIOC->BSRR = GPIO_BSRR_BS4;       /* PC4 HIGH             */
        volatile uint32_t i;
        for (i = 0; i < 84; i++) { __NOP(); }
        GPIOC->BSRR = GPIO_BSRR_BR4;       /* PC4 LOW              */
    }
}

/* ---------- Part D: DMA UART receive ---------- */
#define DMA_RX_BUF_SIZE  256
static uint8_t ucDmaRxBuf[DMA_RX_BUF_SIZE]
    __attribute__((section(".sram")));        /* NOT in CCM RAM!      */
static StreamBufferHandle_t xUartStream;
static volatile uint16_t usLastDmaPos;

void USART2_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;

    if (USART2->SR & USART_SR_IDLE)
    {
        (void)USART2->DR;                   /* Clear IDLE flag      */

        uint16_t usCurrPos = DMA_RX_BUF_SIZE
                           - (uint16_t)DMA1_Stream5->NDTR;

        if (usCurrPos != usLastDmaPos)
        {
            if (usCurrPos > usLastDmaPos)
            {
                /* Contiguous block */
                xStreamBufferSendFromISR(
                    xUartStream,
                    &ucDmaRxBuf[usLastDmaPos],
                    usCurrPos - usLastDmaPos,
                    &xWoken);
            }
            else
            {
                /* Wrap-around: send tail, then head */
                xStreamBufferSendFromISR(
                    xUartStream,
                    &ucDmaRxBuf[usLastDmaPos],
                    DMA_RX_BUF_SIZE - usLastDmaPos,
                    &xWoken);
                if (usCurrPos > 0)
                {
                    xStreamBufferSendFromISR(
                        xUartStream,
                        &ucDmaRxBuf[0],
                        usCurrPos,
                        &xWoken);
                }
            }
            usLastDmaPos = usCurrPos;
        }
    }

    portYIELD_FROM_ISR(xWoken);
}

/* ---------- Handler Tasks ---------- */
static void vButtonTask(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        xSemaphoreTake(xButtonSemaphore, portMAX_DELAY);
        uart_printf("[BTN] Latency: %lu cycles (%lu ns)  "
                    "ISR duration: %lu cycles (%lu ns)\r\n",
                    (unsigned long)ulLatencyCycles,
                    (unsigned long)ulLatencyCycles * 1000UL / 168UL,
                    (unsigned long)(ulIsrExitTimestamp
                                    - ulIsrEntryTimestamp),
                    (unsigned long)(ulIsrExitTimestamp
                                    - ulIsrEntryTimestamp)
                    * 1000UL / 168UL);
    }
}

static void vAdcProcessingTask(void *pvParameters)
{
    (void)pvParameters;
    uint16_t  usSample;
    uint32_t  ulSum = 0;
    uint16_t  usCount = 0;

    for (;;)
    {
        if (xQueueReceive(xAdcQueue, &usSample, portMAX_DELAY))
        {
            ulSum += usSample;
            usCount++;
            if (usCount >= 16)
            {
                uint16_t usAvg = (uint16_t)(ulSum / 16);
                uart_printf("[ADC] Average (16 samples): %u\r\n",
                            (unsigned)usAvg);
                ulSum = 0;
                usCount = 0;
            }
        }
    }
}

static void vUartDmaTask(void *pvParameters)
{
    (void)pvParameters;
    uint8_t ucBuf[128];

    for (;;)
    {
        size_t xLen = xStreamBufferReceive(xUartStream, ucBuf,
                                           sizeof(ucBuf),
                                           portMAX_DELAY);
        if (xLen > 0)
        {
            uart_printf("[DMA-UART] Received %u bytes\r\n",
                        (unsigned)xLen);
        }
    }
}

static void vStatsTask(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
        uart_printf("[STATS] ADC ISR count: %lu\r\n",
                    (unsigned long)ulAdcIsrCount);
    }
}

/* ---------- Main ---------- */
int main(void)
{
    /* BSP init: clocks, UART, GPIO -- per your board support package */
    /* HAL_Init(); SystemClock_Config(); UART_Init(); GPIO_Init(); */

    DWT_Init();
    TIM5_Capture_Init();

    uart_printf("\r\n=== Lab 8: Interrupt Latency and ISR Design ===\r\n");

    /* Create synchronization objects */
    xButtonSemaphore = xSemaphoreCreateBinary();
    xAdcQueue        = xQueueCreate(ADC_QUEUE_LEN, sizeof(uint16_t));
    xUartStream      = xStreamBufferCreate(512, 1);

    /* Configure EXTI0 for PA0 (user button), priority 6 */
    /* (HAL or register-level EXTI/SYSCFG configuration here) */
    NVIC_SetPriority(EXTI0_IRQn, 6);
    NVIC_EnableIRQ(EXTI0_IRQn);

    /* Configure nesting timer interrupts (UIE set in each DIER;
     * only TIM2 counting, e.g. a 10 ms period) */
    NVIC_SetPriority(TIM2_IRQn, 10);
    NVIC_SetPriority(TIM3_IRQn, 8);
    NVIC_SetPriority(TIM4_IRQn, 6);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);

    /* Configure USART2 DMA receive -- see Section 8.7 */
    /* UART_DMA_Init(); */

    /* Create tasks */
    xTaskCreate(vButtonTask,        "Button",   512, NULL, 3, NULL);
    xTaskCreate(vAdcProcessingTask, "ADCProc",  512, NULL, 3, NULL);
    xTaskCreate(vUartDmaTask,       "DmaUart",  512, NULL, 3, NULL);
    xTaskCreate(vStatsTask,         "Stats",    256, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) { }  /* Should never reach here */
    return 0;
}
