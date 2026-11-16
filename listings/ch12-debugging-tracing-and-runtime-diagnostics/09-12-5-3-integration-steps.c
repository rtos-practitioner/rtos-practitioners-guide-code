/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.5 Percepio Tracealyzer Integration > 12.5.3 Integration Steps
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include "trcRecorder.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    vTraceEnable(TRC_START);   /* Initialize and start recording */

    /* Create tasks, queues, etc. */
    xTaskCreate(vMotorTask, "Motor", 256, NULL, 4, NULL);
    xTaskCreate(vCommTask,  "Comm",  512, NULL, 2, NULL);

    vTaskStartScheduler();
    for (;;) {}
}
