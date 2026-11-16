/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 2: Development Environment, Project Layout, and Board Bring-Up
 * 2.4 The Boot Sequence Explained > Phase 3: System Initialization and main()
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
int main(void) {
    /* 1. Configure flash latency BEFORE increasing clock speed */
    HAL_Init();   /* Sets flash wait states, enables prefetch */

    /* 2. Configure clock tree: HSE -> PLL -> 168 MHz */
    SystemClock_Config();

    /* 3. Initialize peripherals needed for basic operation */
    GPIO_Init();    /* Debug LEDs, timing pins */
    UART_Init();    /* Debug console */

    /* 4. Create all RTOS kernel objects */
    xHeartbeatQueue = xQueueCreate(4, sizeof(uint32_t));
    configASSERT(xHeartbeatQueue != NULL);

    /* 5. Create all tasks */
    BaseType_t ret;
    ret = xTaskCreate(vHeartbeatTask, "HB",  256, NULL, 2, NULL);
    configASSERT(ret == pdPASS);

    ret = xTaskCreate(vMonitorTask,   "Mon", 512, NULL, 4, NULL);
    configASSERT(ret == pdPASS);

    /* 6. Start the scheduler (does not return on success) */
    vTaskStartScheduler();

    /* 7. Execution reaches here only if scheduler start failed */
    configASSERT(0);   /* Trap: insufficient heap for idle task */
    for (;;) {}
}
