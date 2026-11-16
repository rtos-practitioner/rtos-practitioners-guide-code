/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.3 Creating Tasks > 3.3.4 Code Example: Creating Three Tasks
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Task function prototypes */
void vTaskHigh(void *pvParameters);
void vTaskMedium(void *pvParameters);
void vTaskLow(void *pvParameters);

/* Task handles (optional, needed for runtime control) */
TaskHandle_t xHighHandle = NULL;
TaskHandle_t xMedHandle  = NULL;
TaskHandle_t xLowHandle  = NULL;

int main(void)
{
    /* Hardware initialization (clocks, GPIO, etc.) */
    HAL_Init();
    SystemClock_Config();

    /* Create three tasks at different priorities */
    xTaskCreate(vTaskHigh,   "High",   256, NULL, 3, &xHighHandle);
    xTaskCreate(vTaskMedium, "Medium", 256, NULL, 2, &xMedHandle);
    xTaskCreate(vTaskLow,    "Low",    256, NULL, 1, &xLowHandle);

    /* Start the scheduler — this call never returns */
    vTaskStartScheduler();

    /* Execution will only reach here if there is insufficient heap */
    for (;;);
}

void vTaskHigh(void *pvParameters)
{
    (void)pvParameters;   /* Suppress unused-parameter warning */
    for (;;)
    {
        /* Perform high-priority work (e.g., control loop) */
        ProcessControlLoop();
        vTaskDelay(pdMS_TO_TICKS(10));  /* Block for 10 ms */
    }
}

void vTaskMedium(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        /* Medium-priority work (e.g., communication) */
        HandleComms();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void vTaskLow(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        /* Low-priority housekeeping (e.g., logging) */
        UpdateLog();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
