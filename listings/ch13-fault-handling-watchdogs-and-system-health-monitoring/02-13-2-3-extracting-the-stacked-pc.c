/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.2 ARM Cortex-M Fault Handlers > 13.2.3 Extracting the Stacked PC
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* Naked trampoline: no compiler prologue may touch the stack before
   the stack pointer is captured. */
__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile(
        "TST   LR, #4          \n"  /* Test bit 2 of EXC_RETURN */
        "ITE   EQ               \n"
        "MRSEQ R0, MSP          \n"  /* Fault from handler: use MSP */
        "MRSNE R0, PSP          \n"  /* Fault from task:    use PSP */
        "B     hard_fault_handler_c \n"
    );
}

__attribute__((used)) void hard_fault_handler_c(uint32_t *stack_frame)
{
    volatile uint32_t r0   = stack_frame[0];
    volatile uint32_t r1   = stack_frame[1];
    volatile uint32_t r2   = stack_frame[2];
    volatile uint32_t r3   = stack_frame[3];
    volatile uint32_t r12  = stack_frame[4];
    volatile uint32_t lr   = stack_frame[5];
    volatile uint32_t pc   = stack_frame[6];   /* Faulting instruction */
    volatile uint32_t psr  = stack_frame[7];

    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t hfsr = SCB->HFSR;
    volatile uint32_t mmfar = SCB->MMFAR;
    volatile uint32_t bfar = SCB->BFAR;

    /* Log to UART, backup SRAM, or ITM (Chapter 12) */
    fault_log_to_backup_sram(pc, lr, cfsr, hfsr);

    /* Enter safe state or reset */
    safe_state();
}
