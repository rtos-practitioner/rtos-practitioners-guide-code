/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.6 SEGGER SystemView Integration > 12.6.5 Custom Markers
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#define SYSVIEW_EVT_ADC_COMPLETE   (32u)  /* IDs 0-31 reserved by SystemView */

void ADC_IRQHandler(void)
{
    SEGGER_SYSVIEW_RecordEnterISR();

    uint16_t sample = ADC1->DR;
    SEGGER_SYSVIEW_RecordU32(SYSVIEW_EVT_ADC_COMPLETE, sample);

    /* ... process sample ... */

    SEGGER_SYSVIEW_RecordExitISR();
}
