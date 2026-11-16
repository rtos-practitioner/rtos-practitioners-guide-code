/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.3 Static vs Dynamic Allocation > How Static Allocation Works
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Application-provided memory */
static StaticTask_t xTaskTCB;
static StackType_t  xTaskStack[256];  /* 256 words = 1024 bytes */

TaskHandle_t xHandle = xTaskCreateStatic(
    vMyTaskFunction,    /* Task entry point      */
    "MyTask",           /* Name for debugging     */
    256,                /* Stack depth in words   */
    NULL,               /* Parameter              */
    tskIDLE_PRIORITY+1, /* Priority               */
    xTaskStack,         /* Stack buffer           */
    &xTaskTCB           /* TCB buffer             */
);
/* xHandle is guaranteed non-NULL---creation cannot fail */
