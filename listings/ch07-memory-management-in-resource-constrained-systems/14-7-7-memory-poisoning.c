/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.7 Memory Debugging Techniques > Memory Poisoning
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
void vPortFreeWithPoison(void *pv, size_t xSize)
{
    memset(pv, 0xDE, xSize);  /* Poison the block */
    vPortFree(pv);
}
