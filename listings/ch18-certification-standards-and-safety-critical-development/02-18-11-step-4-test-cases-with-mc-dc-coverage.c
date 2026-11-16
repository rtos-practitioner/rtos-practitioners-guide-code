/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 18: Certification, Standards, and Safety-Critical Development
 * 18.11 Lab 18: Creating a Certification Evidence Package > Step 4: Test Cases with MC/DC Coverage
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
if ((fTemperature >= fHighThreshold) && (xAlarmActive == pdFALSE)) {
    vActivateAlarm();
}
