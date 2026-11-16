/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.7 Runtime Diagnostics APIs > 12.7.3 uxTaskGetStackHighWaterMark()
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
UBaseType_t hwm = uxTaskGetStackHighWaterMark(xMotorTaskHandle);
if (hwm < 32) {
    /* Dangerously low — increase stack allocation */
    FR_Record(FR_EVT_STACK_LOW, MOTOR_TASK_ID, (uint16_t)hwm);
}
