/* board.h - QEMU netduinoplus2 (STM32F405, Cortex-M4F) board support for the book harness */
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/* Exit codes reported to the host through QEMU semihosting */
#define BOARD_EXIT_OK          0   /* ran to the end of its time window, no fault   */
#define BOARD_EXIT_FAULT       2   /* HardFault / MemManage / BusFault / UsageFault */
#define BOARD_EXIT_STACK_OVF   3   /* configCHECK_FOR_STACK_OVERFLOW hook fired     */
#define BOARD_EXIT_MALLOC      4   /* pvPortMalloc failed (heap exhausted)          */
#define BOARD_EXIT_ASSERT      5   /* configASSERT() failed                         */
#define BOARD_EXIT_SCHED_FAIL  6   /* vTaskStartScheduler() returned                */
#define BOARD_EXIT_UNIT_DONE   7   /* unit finished before its time window          */

void     board_init(void);
void     board_exit(int code) __attribute__((noreturn));
void     board_assert_failed(const char *file, int line) __attribute__((noreturn));
uint32_t board_cycles(void);                 /* 168 MHz free-running cycle counter */
void     board_arm_deadline(uint32_t ms, int tim);   /* virtual-time deadline on TIM2/3/5 */
void     board_puts(const char *s);                  /* raw USART1 output, bypasses _write()  */
void     board_put_dec(uint32_t v);
void     board_put_hex(uint32_t v);

#endif
