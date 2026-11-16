/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.7 Memory Debugging Techniques > Heap Allocation Tracing
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
extern void vTraceAlloc(void *pvAddress, size_t xSize);
extern void vTraceFree(void *pvAddress, size_t xSize);

#define traceMALLOC(pvAddress, uiSize) \
    vTraceAlloc(pvAddress, uiSize)
#define traceFREE(pvAddress, uiSize) \
    vTraceFree(pvAddress, uiSize)
