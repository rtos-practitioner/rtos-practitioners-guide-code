/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.9 System Startup and Shutdown > 17.9.2 Code Example: Multi-Phase Startup with Event Group Sync
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define SENSOR_READY_BIT   (1 << 0)
#define COMM_READY_BIT     (1 << 1)
#define UI_READY_BIT       (1 << 2)
#define ALL_READY_BITS     (SENSOR_READY_BIT | COMM_READY_BIT | \
                            UI_READY_BIT)

static EventGroupHandle_t xStartupSync;

int main(void)
{
    /* Phase 0: Hardware */
    HAL_Init();
    SystemClock_Config();

    /* Phase 1: Drivers */
    vUartDriverInit();
    vSpiDriverInit();
    vAdcDriverInit();

    /* Phase 2: OS objects */
    xStartupSync = xEventGroupCreate();
    configASSERT(xStartupSync != NULL);
    vEventDispatcherInit();       /* Queue exists before EXTI0 fires */

    vLedControllerCreate();       /* Active object */
    vSensorManagerCreate();
    vCommManagerCreate();

    /* Phase 3: Start scheduler */
    vTaskStartScheduler();

    /* Should never reach here */
    for (;;) {}
}

/* Inside sensor manager task */
void vSensorManagerTask(void *pvParam)
{
    /* Phase 4: Task-level init */
    vCalibrateSensor();
    vInitFilterCoefficients();

    /* Signal readiness */
    xEventGroupSetBits(xStartupSync, SENSOR_READY_BIT);

    /* Wait for all subsystems to be ready */
    EventBits_t uxBits = xEventGroupWaitBits(xStartupSync,
                            ALL_READY_BITS, pdFALSE, pdTRUE,
                            pdMS_TO_TICKS(5000));

    if ((uxBits & ALL_READY_BITS) != ALL_READY_BITS) {
        /* Timeout: report which subsystems never became ready,
           then run degraded or request a controlled reset */
        vReportStartupFault(ALL_READY_BITS & ~uxBits);
        vEnterDegradedMode();
    }

    /* Phase 5: Normal operation */
    for (;;) {
        /* ... process events ... */
    }
}
