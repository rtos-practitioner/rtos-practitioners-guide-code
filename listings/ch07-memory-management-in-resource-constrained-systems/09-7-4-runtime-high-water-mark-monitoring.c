/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.4 Stack Sizing and Overflow Detection > Runtime High-Water Mark Monitoring
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vStackMonitorTask(void *pvParameters)
{
    TaskHandle_t xTasks[8];
    char         cName[configMAX_TASK_NAME_LEN];

    for (;;)
    {
        UBaseType_t uxCount = uxTaskGetNumberOfTasks();
        TaskStatus_t *pxStatusArray = pvPortMalloc(
            uxCount * sizeof(TaskStatus_t));

        if (pxStatusArray != NULL)
        {
            uxCount = uxTaskGetSystemState(pxStatusArray,
                                           uxCount, NULL);
            for (UBaseType_t i = 0; i < uxCount; i++)
            {
                UBaseType_t uxHWM = uxTaskGetStackHighWaterMark(
                    pxStatusArray[i].xHandle);
                /* Log: task name and remaining stack words */
                uart_printf("[STACK] %-12s HWM: %4lu words (%lu bytes)\r\n",
                    pxStatusArray[i].pcTaskName,
                    uxHWM, uxHWM * sizeof(StackType_t));
            }
            vPortFree(pxStatusArray);
        }
        vTaskDelay(pdMS_TO_TICKS(10000));  /* Log every 10 seconds */
    }
}
