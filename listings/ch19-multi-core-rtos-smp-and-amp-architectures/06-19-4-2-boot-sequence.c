/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.4 AMP with FreeRTOS > 19.4.2 Boot Sequence
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
/*
 * STM32H755 AMP Boot — Cortex-M7 main.c
 *
 * M7 initializes clocks, shared resources, and IPC,
 * then releases the M4 core from reset.
 */

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "amp_ipc.h"

/* Located in SRAM3 — shared, non-cacheable */
volatile SharedIPC_t *pxSharedIPC =
    (volatile SharedIPC_t *)0x30040000UL;

static void vM7ControlTask(void *pvParameters)
{
    for (;;) {
        /* Real-time motor control at 10 kHz */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        motor_foc_update(&motor_state);

        /* Send telemetry to M4 via shared ring buffer */
        ipc_ring_push(&pxSharedIPC->telemetry_ring,
                       &motor_state.telemetry,
                       sizeof(Telemetry_t));

        /* SRAM3 is MPU non-cacheable, so a barrier suffices; a
           cacheable buffer would also need SCB_CleanDCache_by_Addr() */
        __DMB();

        /* Signal M4 via hardware semaphore interrupt */
        HAL_HSEM_FastTake(HSEM_ID_IPC_NOTIFY);
        HAL_HSEM_Release(HSEM_ID_IPC_NOTIFY, 0);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();   /* 480 MHz M7, 240 MHz M4 */

    /* Configure MPU: SRAM3 as non-cacheable, shared */
    mpu_configure_shared_region(0x30040000UL, 32 * 1024);

    /* Initialize shared IPC region before M4 boots */
    ipc_shared_init(pxSharedIPC);

    /* Enable HSEM clock; only the M4 enables the notification */
    __HAL_RCC_HSEM_CLK_ENABLE();

    /* Release Cortex-M4 from reset */
    HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);

    xTaskCreate(vM7ControlTask, "M7Ctrl", 512, NULL, 5, NULL);
    vTaskStartScheduler();
    for (;;) {}
}
