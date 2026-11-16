/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.4 AMP with FreeRTOS > 19.4.2 Boot Sequence
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
/*
 * STM32H755 AMP Boot — Cortex-M4 main.c
 *
 * M4 runs FreeRTOS with a data processing task that
 * receives telemetry from M7 via shared memory.
 */

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "amp_ipc.h"

volatile SharedIPC_t *pxSharedIPC =
    (volatile SharedIPC_t *)0x30040000UL;

static TaskHandle_t xProcessTaskHandle;

void HSEM2_IRQHandler(void)
{
    HAL_HSEM_IRQHandler();
    /* The HAL disarms the notification on each event; re-arm it */
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(
        HSEM_ID_IPC_NOTIFY));
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xProcessTaskHandle,
                           &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void vM4ProcessTask(void *pvParameters)
{
    Telemetry_t telemetry;
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /* Order the notification before reading shared data
           (the M4 has no data cache, so no invalidate is needed) */
        __DMB();
        while (ipc_ring_pop(&pxSharedIPC->telemetry_ring,
                             &telemetry,
                             sizeof(Telemetry_t))) {
            data_log_store(&telemetry);
            diagnostics_update(&telemetry);
        }
    }
}

int main(void)
{
    HAL_Init();
    /* M4 clock already configured by M7 */

    /* Wait for M7 to initialize shared IPC */
    while (pxSharedIPC->initialized != 0xDEADBEEFU) {
        __NOP();
    }

    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(
        HSEM_ID_IPC_NOTIFY));
    HAL_NVIC_EnableIRQ(HSEM2_IRQn);

    xTaskCreate(vM4ProcessTask, "M4Proc", 512, NULL, 3,
                &xProcessTaskHandle);
    vTaskStartScheduler();
    for (;;) {}
}
