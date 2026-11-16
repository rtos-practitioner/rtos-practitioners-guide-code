/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.2 printf-Style Debugging > 12.2.3 ITM — Instrumentation Trace Macrocell
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* ITM printf redirection: retarget _write() or fputc() */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        /* Wait until stimulus port 0 is ready */
        while (ITM->PORT[0].u32 == 0)
            ;
        ITM->PORT[0].u8 = (uint8_t)ptr[i];
    }
    return len;
}
