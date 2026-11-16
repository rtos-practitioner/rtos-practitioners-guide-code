/* shim/core_cm4.h - wraps the CMSIS Cortex-M4 core header for QEMU.
 *
 * 1) DWT cycle counter. QEMU's Cortex-M model has no DWT (reads as 0 forever)
 *    but the book's samples all use DWT->CYCCNT / DWT->CTRL, so this wrapper
 *    re-points the DWT symbol at TIM4 with a layout-compatible overlay:
 *
 *        DWT->CTRL   (offset 0x00)  ==  TIM4->CR1  (CEN bit 0 == CYCCNTENA)
 *        DWT->CYCCNT (offset 0x24)  ==  TIM4->CNT
 *
 *    QEMU's timers count virtual nanoseconds / (PSC+1) and the SoC pins their
 *    clock at 1 GHz, so board_init() sets PSC=5: CYCCNT ticks at 166.67 MHz,
 *    0.8 % below the real 168 MHz core clock. Book code is NOT modified.
 *
 * 2) __WFI/__WFE become NOPs. With -icount ...,sleep=off QEMU 6.2 mis-times
 *    SysTick (2x period) whenever the guest halts in WFI; spinning instead
 *    keeps virtual time purely instruction-driven, i.e. fully deterministic.
 *    The architecture allows WFI to return immediately, so code that waits in
 *    a loop behaves identically.
 */
#ifndef QEMU_SHIM_CORE_CM4_H
#define QEMU_SHIM_CORE_CM4_H

#include_next <core_cm4.h>

#include <stdint.h>

typedef struct {
  volatile uint32_t CTRL;          /* 0x00 -> TIM4->CR1  */
  volatile uint32_t _reserved[8];  /* 0x04..0x20         */
  volatile uint32_t CYCCNT;        /* 0x24 -> TIM4->CNT  */
} QemuDwt_Type;

#undef  DWT
#define DWT ((QemuDwt_Type *)0x40000800UL)   /* TIM4_BASE */

/* 3) QEMU 6.2's NVIC implements 8 priority bits (the real STM32F4 has 4) and has no property
 *    to change it. On an 8-bit NVIC FreeRTOS requires AIRCR.PRIGROUP == 0, so every request
 *    for a grouping (HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4), CMSIS directly) is
 *    mapped to 0. NVIC_EncodePriority() still yields identical register values because the
 *    4 implemented STM32 bits stay the top nibble of the priority byte.
 */
#undef  NVIC_SetPriorityGrouping
#define NVIC_SetPriorityGrouping(g) __NVIC_SetPriorityGrouping(0u)

#undef  __WFI
#define __WFI() __ASM volatile ("nop":::"memory")
#undef  __WFE
#define __WFE() __ASM volatile ("nop":::"memory")

#endif
