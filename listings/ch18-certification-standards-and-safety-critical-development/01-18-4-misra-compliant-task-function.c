/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 18: Certification, Standards, and Safety-Critical Development
 * 18.4 MISRA C Compliance > MISRA-Compliant Task Function
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* MISRA C:2012 compliant task function */

/* Rule 8.4: Compatible declaration shall be visible */
extern void vTemperatureMonitorTask(void *pvParameters);

/* Rule 8.13: pvParameters could be const, but FreeRTOS API
   defines the signature as void* — deviation documented */
void vTemperatureMonitorTask(void *pvParameters)
{
    /* Rule 21.3: No dynamic allocation — use stack variables */
    TickType_t xLastWakeTime;
    float fTemperature;
    float fThreshold;
    BaseType_t xAlarmActive;

    /* Rule 11.4 deviation: cast required for hardware register access */
    /* Deviation documented in project deviation log, entry DEV-0042 */

    xLastWakeTime = xTaskGetTickCount();
    fThreshold = 85.0F;   /* Rule 10.3: float constant, no double */
    xAlarmActive = pdFALSE;

    /* Rule 2.1: loop body is reachable */
    for (;;)   /* Rule 14.2: compliant for-loop for infinite loop */
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100U));

        fTemperature = fReadTemperatureSensor();

        if (fTemperature >= fThreshold)    /* Rule 13.5: no side effects */
        {
            if (xAlarmActive == pdFALSE)
            {
                vActivateAlarm();
                xAlarmActive = pdTRUE;
            }
            /* else: alarm already active, no action */
        }
        else
        {
            if (xAlarmActive == pdTRUE)
            {
                vDeactivateAlarm();
                xAlarmActive = pdFALSE;
            }
            /* else: alarm already inactive, no action */
        }
    }

    /* Rule 2.1: this point is unreachable — acceptable for
       RTOS tasks per project deviation DEV-0003 */
}
