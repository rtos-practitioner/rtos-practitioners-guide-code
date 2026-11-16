/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.7 Task Notifications as Lightweight IPC > General-Purpose Notification
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify,
                        uint32_t ulValue,
                        eNotifyAction eAction );
