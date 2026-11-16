/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.2 Core M7 Firmware (Producer)
 * Verification: compiled; needs real hardware (not modelled by QEMU)
 * SPDX-License-Identifier: MIT
 */
/* main_m7.c — Cortex-M7 firmware (Core 0) */

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ipc_shared.h"

#define SHARED_IPC_ADDR  0x30040000UL

volatile SharedIPC_t *pxIPC =
    (volatile SharedIPC_t *)SHARED_IPC_ADDR;

static uint16_t usSequence = 0;

/* GPIO pin toggle for oscilloscope latency measurement */
#define LATENCY_PIN_PORT   GPIOB
#define LATENCY_PIN        GPIO_PIN_0

static void vProducerTask(void *pv)
{
    TickType_t xLastWake = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWake, pdMS_TO_TICKS(1)); /* 1 kHz */

        SensorSample_t sample;
        sample.timestamp_us = DWT->CYCCNT / (SystemCoreClock / 1000000);
        sample.sensor_value = (int16_t)(HAL_ADC_GetValue(&hadc1) & 0xFFFF);
        sample.sequence     = usSequence++;

        /* Toggle pin HIGH — start of latency measurement */
        HAL_GPIO_WritePin(LATENCY_PIN_PORT, LATENCY_PIN, GPIO_PIN_SET);

        if (ipc_ring_push(&pxIPC->sensor_ring, &sample) == 0) {
            /* Ring stores complete before the notification (SRAM3
               is MPU non-cacheable, so no cache clean is needed) */
            __DMB();
            /* Notify M4 via HSEM release */
            HAL_HSEM_FastTake(HSEM_ID_IPC_NOTIFY);
            HAL_HSEM_Release(HSEM_ID_IPC_NOTIFY, 0);
        }

        HAL_GPIO_WritePin(LATENCY_PIN_PORT, LATENCY_PIN, GPIO_PIN_RESET);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();

    /* Enable DWT cycle counter for timestamps */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* Configure shared region as non-cacheable */
    mpu_configure_shared_region(SHARED_IPC_ADDR, 32 * 1024);

    /* Initialize shared IPC structure */
    memset((void *)pxIPC, 0, sizeof(SharedIPC_t));
    __DMB();
    pxIPC->initialized = 0xDEADBEEFUL;

    /* Enable HSEM */
    __HAL_RCC_HSEM_CLK_ENABLE();

    /* Release M4 from reset */
    HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);

    /* Wait for M4 to signal ready */
    while (pxIPC->m4_ready != 0xCAFECAFEUL) { __NOP(); }

    xTaskCreate(vProducerTask, "Producer", 256, NULL, 4, NULL);
    vTaskStartScheduler();
    for (;;) {}
}
