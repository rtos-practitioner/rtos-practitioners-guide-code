/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.8 Post-Mortem Analysis > 12.8.3 Post-Reset Recovery
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    /* Enable backup SRAM access (also required by the fault handler) */
    BackupSRAM_Enable();

    if (g_crashValid == 0xDEADBEEF) {
        g_crashValid = 0;
        printf("!!! Previous crash detected !!!\n");
        printf("PC  = 0x%08lX  (faulting instruction)\n",
               (unsigned long)g_crashCtx.pc);
        printf("LR  = 0x%08lX  (return address)\n",
               (unsigned long)g_crashCtx.lr);
        printf("CFSR= 0x%08lX\n",
               (unsigned long)g_crashCtx.cfsr);
        if (g_crashCtx.cfsr & 0x8000)  /* BFSR.BFARVALID */
            printf("BFAR= 0x%08lX\n",
                   (unsigned long)g_crashCtx.bfar);
        if (g_crashCtx.cfsr & 0x80)    /* MMFSR.MMARVALID */
            printf("MMFAR= 0x%08lX\n",
                   (unsigned long)g_crashCtx.mmfar);

        /* Dump flight recorder from previous run */
        FR_Dump();
    }

    FR_Init();
    /* ... create tasks and other initialization ... */

    vTaskStartScheduler();
    for (;;) {}   /* Not reached */
}
