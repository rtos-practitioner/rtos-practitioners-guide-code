/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 3: Tasks, Priorities, and the Scheduler
 * 3.9 Starvation and Design Guidelines > 3.9.1 How Starvation Occurs
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vTaskA(void *pvParameters)
{
    for (;;)
    {
        ReadSensor();
        ComputeOutput();
        DriveActuator();
        /* No vTaskDelay(), no blocking call — CPU never released */
    }
}
