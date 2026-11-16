/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.8 Post-Mortem Analysis > 12.8.2 Hard Fault Handler
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* Place in a .c file, not in FreeRTOSConfig.h */

typedef struct {
    uint32_t r0, r1, r2, r3;
    uint32_t r12, lr, pc, psr;
    uint32_t cfsr, hfsr, mmfar, bfar;
} CrashContext;

/* Stored in backup SRAM to survive reset */
__attribute__((section(".backup_sram")))
volatile CrashContext g_crashCtx;

__attribute__((section(".backup_sram")))
volatile uint32_t g_crashValid;

void HardFault_Handler_C(uint32_t *stackFrame)
{
    g_crashCtx.r0   = stackFrame[0];
    g_crashCtx.r1   = stackFrame[1];
    g_crashCtx.r2   = stackFrame[2];
    g_crashCtx.r3   = stackFrame[3];
    g_crashCtx.r12  = stackFrame[4];
    g_crashCtx.lr   = stackFrame[5];
    g_crashCtx.pc   = stackFrame[6];
    g_crashCtx.psr  = stackFrame[7];

    g_crashCtx.cfsr = SCB->CFSR;
    g_crashCtx.hfsr = SCB->HFSR;
    g_crashCtx.mmfar = SCB->MMFAR;
    g_crashCtx.bfar  = SCB->BFAR;

    g_crashValid = 0xDEADBEEF;

    /* Record to flight recorder */
    FR_Record(FR_EVT_HARD_FAULT, 0xFF,
              (uint16_t)(g_crashCtx.cfsr & 0xFFFF));

    /* Optional: trigger a soft reset to allow post-mortem dump */
    NVIC_SystemReset();
}

/* Assembly wrapper to extract correct stack pointer */
__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile(
        "TST   LR, #4          \n"  /* Test bit 2 of EXC_RETURN */
        "ITE   EQ               \n"
        "MRSEQ R0, MSP          \n"  /* Fault used MSP */
        "MRSNE R0, PSP          \n"  /* Fault used PSP */
        "B     HardFault_Handler_C \n"
    );
}
