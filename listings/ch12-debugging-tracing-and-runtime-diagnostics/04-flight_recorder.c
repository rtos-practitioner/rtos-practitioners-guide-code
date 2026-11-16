/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.4 FreeRTOS Trace Hooks > 12.4.2 The Flight Recorder Pattern
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* flight_recorder.c */
#include "flight_recorder.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/* Place in backup SRAM or .noinit section to survive reset */
__attribute__((section(".noinit")))
static FlightRecorder fr;

#define FR_MAGIC  0x464C5400  /* "FLT\0" */

void FR_Init(void)
{
    /* Enable DWT cycle counter */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;

    /* Preserve buffer across soft reset if magic is valid */
    if (fr.magic != FR_MAGIC) {
        fr.magic = FR_MAGIC;
        fr.head  = 0;
        fr.count = 0;
    }
}

void FR_Record(uint8_t event, uint8_t task_id, uint16_t detail)
{
    uint32_t idx = fr.head & (FR_BUFFER_SIZE - 1);

    fr.entries[idx].timestamp = DWT->CYCCNT;
    fr.entries[idx].event     = event;
    fr.entries[idx].task_id   = task_id;
    fr.entries[idx].detail    = detail;

    fr.head++;
    fr.count++;
}

void FR_Dump(void)
{
    uint32_t start, n;

    if (fr.count >= FR_BUFFER_SIZE) {
        start = fr.head & (FR_BUFFER_SIZE - 1);
        n     = FR_BUFFER_SIZE;
    } else {
        start = 0;
        n     = fr.count;
    }

    printf("=== Flight Recorder: %lu events ===\n",
           (unsigned long)fr.count);
    printf(" # | Cycles       | Evt  | Task | Detail\n");
    printf("---|--------------|------|------|-------\n");

    for (uint32_t i = 0; i < n; i++) {
        uint32_t idx = (start + i) & (FR_BUFFER_SIZE - 1);
        FlightRecordEntry *e = &fr.entries[idx];
        printf("%3lu| %12lu | 0x%02X | %4u | 0x%04X\n",
               (unsigned long)i,
               (unsigned long)e->timestamp,
               e->event, e->task_id, e->detail);
    }
}
