/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.3 Core M4 Firmware (Consumer)
 * Verification: compiled; needs real hardware (not modelled by QEMU)
 * SPDX-License-Identifier: MIT
 */
/* main_m4.c — Cortex-M4 firmware (Core 1) */

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ipc_shared.h"

#define SHARED_IPC_ADDR  0x30040000UL

volatile SharedIPC_t *pxIPC =
    (volatile SharedIPC_t *)SHARED_IPC_ADDR;

static TaskHandle_t xConsumerHandle;

/* Statistics */
static uint32_t ulMessagesReceived = 0;
static uint32_t ulSequenceErrors   = 0;
static uint16_t usExpectedSeq      = 0;

/* GPIO for latency measurement — toggle LOW on receive */
#define LATENCY_PIN_PORT   GPIOB
#define LATENCY_PIN        GPIO_PIN_1

void HSEM2_IRQHandler(void)
{
    HAL_HSEM_IRQHandler();
    /* The HAL disarms the notification on each event; re-arm it */
    HAL_HSEM_ActivateNotification(
        __HAL_HSEM_SEMID_TO_MASK(HSEM_ID_IPC_NOTIFY));
    BaseType_t xWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xConsumerHandle, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}

static void vConsumerTask(void *pv)
{
    SensorSample_t sample;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /* Order the notification before reading shared data */
        __DMB();

        while (ipc_ring_pop(&pxIPC->sensor_ring, &sample) == 0) {
            /* Toggle pin LOW — end of latency measurement */
            HAL_GPIO_WritePin(LATENCY_PIN_PORT, LATENCY_PIN,
                              GPIO_PIN_RESET);

            ulMessagesReceived++;

            /* Verify sequence continuity */
            if (sample.sequence != usExpectedSeq) {
                ulSequenceErrors++;
            }
            usExpectedSeq = sample.sequence + 1;

            /* Process sample (filtering, logging, etc.) */
            process_sample(&sample);

            HAL_GPIO_WritePin(LATENCY_PIN_PORT, LATENCY_PIN,
                              GPIO_PIN_SET);
        }
    }
}

int main(void)
{
    HAL_Init();

    /* Wait for M7 to initialize shared memory */
    while (pxIPC->initialized != 0xDEADBEEFUL) { __NOP(); }

    MX_GPIO_Init();

    /* Enable HSEM notification for this core */
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_HSEM_ActivateNotification(
        __HAL_HSEM_SEMID_TO_MASK(HSEM_ID_IPC_NOTIFY));
    HAL_NVIC_SetPriority(HSEM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(HSEM2_IRQn);

    xTaskCreate(vConsumerTask, "Consumer", 256, NULL, 3,
                &xConsumerHandle);

    /* Signal M7 that M4 is ready */
    __DMB();
    pxIPC->m4_ready = 0xCAFECAFEUL;

    vTaskStartScheduler();
    for (;;) {}
}
