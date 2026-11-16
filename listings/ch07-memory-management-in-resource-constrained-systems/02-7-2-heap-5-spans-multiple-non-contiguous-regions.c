/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.2 FreeRTOS Heap Implementations > heap_5: Spans Multiple Non-Contiguous Regions
 * Verification: compiled; needs real hardware (not modelled by QEMU)
 * SPDX-License-Identifier: MIT
 */
/* Regions must be listed in ascending start-address order */
const HeapRegion_t xHeapRegions[] = {
    { (uint8_t *) 0x10000000, 0x10000 },  /* 64 KB in CCM RAM (lower) */
    { (uint8_t *) 0x20010000, 0x10000 },  /* 64 KB in main SRAM       */
    { NULL, 0 }                            /* Terminator              */
};

vPortDefineHeapRegions(xHeapRegions);
