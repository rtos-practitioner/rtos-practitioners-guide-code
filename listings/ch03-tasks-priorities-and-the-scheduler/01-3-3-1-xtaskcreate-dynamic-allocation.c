/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.3 Creating Tasks > 3.3.1 xTaskCreate() — Dynamic Allocation
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xTaskCreate(
    TaskFunction_t pvTaskCode,       /* Pointer to the task function          */
    const char * const pcName,       /* Human-readable name (debug only)      */
    configSTACK_DEPTH_TYPE usStackDepth, /* Stack size in WORDS (not bytes)   */
    void *pvParameters,              /* Argument passed into task function    */
    UBaseType_t uxPriority,          /* Task priority (0 = lowest)            */
    TaskHandle_t *pxCreatedTask      /* Optional: returns task handle         */
);
