/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.8 Recovery Strategies > 13.8.1 Task Restart
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
static TaskHandle_t comms_handle = NULL;

void recover_task(int task_index)
{
    if (task_index == 1 && comms_handle != NULL) {
        vTaskDelete(comms_handle);
        comms_handle = NULL;

        /* Re-initialize the subsystem state */
        comms_subsystem_init();

        /* Re-create the task */
        xTaskCreate(comms_task, "Comms",
                    COMMS_STACK_SIZE, NULL,
                    COMMS_PRIORITY, &comms_handle);

        log_warning("Task 'Comms' restarted\r\n");
    }
}
