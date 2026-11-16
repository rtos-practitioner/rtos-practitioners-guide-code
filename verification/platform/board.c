/* board.c - QEMU netduinoplus2 (STM32F405 @168 MHz, Cortex-M4F) support layer.
 *
 * Everything the book's samples assume exists on an STM32F407 board but which
 * is NOT part of the book listings lives here as *weak* definitions, so a
 * listing that provides its own version (e.g. vApplicationStackOverflowHook,
 * HardFault_Handler, vConfigureTimerForRunTimeStats) always wins.
 */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "board.h"

#define WEAK __attribute__((weak))

/* ------------------------------------------------------------------ */
/* Semihosting exit: lets the host see PASS/FAIL as QEMU's exit status */
/* ------------------------------------------------------------------ */
void board_exit(int code)
{
    uint32_t blk[2] = { 0x20026u /* ADP_Stopped_ApplicationExit */, (uint32_t)code };
    register uint32_t r0 __asm__("r0") = 0x20;              /* SYS_EXIT_EXTENDED */
    register uint32_t r1 __asm__("r1") = (uint32_t)blk;
    __asm__ volatile("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
    for (;;) { }
}

void _exit(int status) { board_exit(status); }

/* ------------------------------------------------------------------ */
/* Console: USART1 (QEMU maps it to the host's stdout)                 */
/* ------------------------------------------------------------------ */
static void uart_putc(char c)
{
    while (!(USART1->SR & USART_SR_TXE)) { }
    USART1->DR = (uint32_t)c;
}

WEAK int _write(int fd, const char *buf, int len)
{
    (void)fd;
    for (int i = 0; i < len; i++) { uart_putc(buf[i]); }
    return len;
}

WEAK int _read(int fd, char *buf, int len) { (void)fd; (void)buf; (void)len; return 0; }
WEAK int _close(int fd) { (void)fd; return -1; }
WEAK int _fstat(int fd, struct stat *st) { (void)fd; st->st_mode = S_IFCHR; return 0; }
WEAK int _isatty(int fd) { (void)fd; return 1; }
WEAK int _lseek(int fd, int off, int whence) { (void)fd; (void)off; (void)whence; return 0; }
WEAK int _getpid(void) { return 1; }
WEAK int _kill(int pid, int sig) { (void)pid; (void)sig; errno = EINVAL; return -1; }
WEAK void _init(void) { }
WEAK void _fini(void) { }

extern char end;       /* from linker: end of .bss */
extern char _estack;
WEAK void *_sbrk(int incr)
{
    static char *brk;
    char *limit = &_estack - 0x800;          /* keep 2 KB for the MSP stack */
    if (brk == NULL) { brk = &end; }
    if (brk + incr > limit) { errno = ENOMEM; return (void *)-1; }
    char *prev = brk;
    brk += incr;
    return prev;
}

void __malloc_lock(struct _reent *r)   { (void)r; if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) { vTaskSuspendAll(); } }
void __malloc_unlock(struct _reent *r) { (void)r; if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) { (void)xTaskResumeAll(); } }

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("[ASSERT] %s:%d (%s) %s\r\n", file, line, func ? func : "?", expr);
    board_exit(BOARD_EXIT_ASSERT);
}

/* ------------------------------------------------------------------ */
/* Cycle counter / virtual-time deadline                               */
/* TIM2..TIM5 in QEMU count virtual ns / (PSC+1) (SoC fixes clock at   */
/* 1 GHz), and virtual ns == instructions * 8 with -icount shift=3.    */
/* ------------------------------------------------------------------ */
#define CYC_PSC          5u                      /* 1 GHz / 6 = 166.67 MHz "cycle" counter */
uint32_t board_cycles(void) { return TIM4->CNT; }       /* == DWT->CYCCNT via shim */
static uint32_t cycles_to_us(uint32_t c) { return (uint32_t)(((uint64_t)c * (CYC_PSC + 1u)) / 1000u); }

/* The deadline uses a timer the listing under test does not touch (the generator picks 2, 3 or 5). */
static TIM_TypeDef *deadline_tim;

void board_arm_deadline(uint32_t ms, int tim)
{
    static const struct { TIM_TypeDef *t; IRQn_Type irq; } map[] = {
        [2] = { TIM2, TIM2_IRQn }, [3] = { TIM3, TIM3_IRQn }, [5] = { TIM5, TIM5_IRQn } };
    deadline_tim = map[tim].t;
    deadline_tim->CR1  = 0;
    deadline_tim->PSC  = 999u;             /* 1 GHz / 1000 = 1 MHz: ARR counts microseconds */
    deadline_tim->ARR  = ms * 1000u;
    deadline_tim->EGR  = TIM_EGR_UG;
    deadline_tim->SR   = 0;
    deadline_tim->DIER = TIM_DIER_UIE;
    NVIC_SetPriority(map[tim].irq, 0);     /* above every FreeRTOS-managed IRQ */
    NVIC_EnableIRQ(map[tim].irq);
    deadline_tim->CR1  = TIM_CR1_CEN;
}

/* Raw UART output that never goes through the listing's (possibly retargeted) _write()/printf. */
void board_puts(const char *s) { while (*s) { uart_putc(*s++); } }
void board_put_dec(uint32_t v)
{
    char b[11]; int n = 0;
    do { b[n++] = (char)('0' + v % 10u); v /= 10u; } while (v);
    while (n) { uart_putc(b[--n]); }
}
void board_put_hex(uint32_t v)
{
    for (int i = 28; i >= 0; i -= 4) { uart_putc("0123456789abcdef"[(v >> i) & 0xFu]); }
}

extern void harness_deadline(uint32_t interrupted_pc);
void deadline_irq_c(uint32_t *sp)
{
    if (deadline_tim) { deadline_tim->SR = 0; deadline_tim->CR1 = 0; }
    harness_deadline(sp[6]);                /* sp[6] = stacked PC of the interrupted code */
}

#define DEADLINE_HANDLER(NAME)                                                \
    WEAK __attribute__((naked)) void NAME(void)                               \
    {                                                                         \
        __asm volatile("tst lr, #4\n\t"                                       \
                       "ite eq\n\t"                                           \
                       "mrseq r0, msp\n\t"                                    \
                       "mrsne r0, psp\n\t"                                    \
                       "b deadline_irq_c\n\t");                              \
    }
DEADLINE_HANDLER(TIM2_IRQHandler)
DEADLINE_HANDLER(TIM3_IRQHandler)
DEADLINE_HANDLER(TIM5_IRQHandler)

/* ------------------------------------------------------------------ */
/* Board bring-up                                                      */
/* ------------------------------------------------------------------ */
void board_init(void)
{
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;
    SystemCoreClock = 168000000u;
    NVIC_SetPriorityGrouping(3);           /* shim maps every grouping request to 0 (8-bit QEMU NVIC) */
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk;

    TIM4->PSC = CYC_PSC;                   /* free-running cycle counter (DWT shim) */
    TIM4->ARR = 0xFFFFFFFFu;
    TIM4->CR1 = TIM_CR1_CEN;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

/* ------------------------------------------------------------------ */
/* HAL glue: no SysTick takeover, millisecond clock from the cycle ctr */
/* ------------------------------------------------------------------ */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) { (void)TickPriority; return HAL_OK; }
uint32_t HAL_GetTick(void) { return cycles_to_us(board_cycles()) / 1000u; }

/* Weak stand-ins for project functions the listings call but never show */
WEAK void SystemClock_Config(void) { SystemCoreClock = 168000000u; }
WEAK void GPIO_Init(void) { }
WEAK void UART_Init(void) { }
WEAK void Error_Handler(void) { printf("[Error_Handler] called\r\n"); board_exit(BOARD_EXIT_FAULT); }

/* ------------------------------------------------------------------ */
/* FreeRTOS hooks / required callbacks                                 */
/* ------------------------------------------------------------------ */
WEAK void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    printf("[HOOK] stack overflow in task '%s'\r\n", pcTaskName);
    board_exit(BOARD_EXIT_STACK_OVF);
}

WEAK void vApplicationMallocFailedHook(void)
{
    printf("[HOOK] pvPortMalloc failed\r\n");
    board_exit(BOARD_EXIT_MALLOC);
}

WEAK void vApplicationIdleHook(void) { }            /* spin: see shim/core_cm4.h on WFI */
WEAK void vApplicationTickHook(void) { }

void board_assert_failed(const char *file, int line)
{
    printf("[ASSERT] configASSERT failed at %s:%d\r\n", file, line);
    board_exit(BOARD_EXIT_ASSERT);
}

WEAK void vConfigureTimerForRunTimeStats(void) { }
WEAK uint32_t ulGetRunTimeCounterValue(void) { return cycles_to_us(board_cycles()); }   /* 1 MHz */

#if configSUPPORT_STATIC_ALLOCATION
WEAK void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                        StackType_t **ppxIdleTaskStackBuffer,
                                        uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t tcb;
    static StackType_t stack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &tcb;
    *ppxIdleTaskStackBuffer = stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if configUSE_TIMERS
WEAK void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                         StackType_t **ppxTimerTaskStackBuffer,
                                         uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t tcb;
    static StackType_t stack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &tcb;
    *ppxTimerTaskStackBuffer = stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif
#endif

/* ------------------------------------------------------------------ */
/* Fault reporting                                                     */
/* ------------------------------------------------------------------ */
void board_fault_c(uint32_t *sp, uint32_t which)
{
    static const char *const names[] = { "HardFault", "MemManage", "BusFault", "UsageFault" };
    printf("[FAULT] %s pc=0x%08lx lr=0x%08lx CFSR=0x%08lx HFSR=0x%08lx BFAR=0x%08lx MMFAR=0x%08lx\r\n",
           names[which & 3], (unsigned long)sp[6], (unsigned long)sp[5],
           (unsigned long)SCB->CFSR, (unsigned long)SCB->HFSR,
           (unsigned long)SCB->BFAR, (unsigned long)SCB->MMFAR);
    board_exit(BOARD_EXIT_FAULT);
}

#define FAULT_HANDLER(NAME, ID)                                               \
    WEAK __attribute__((naked)) void NAME(void)                               \
    {                                                                         \
        __asm volatile("tst lr, #4\n\t"                                       \
                       "ite eq\n\t"                                           \
                       "mrseq r0, msp\n\t"                                    \
                       "mrsne r0, psp\n\t"                                    \
                       "movs r1, #" #ID "\n\t"                                \
                       "b board_fault_c\n\t");                                \
    }

FAULT_HANDLER(HardFault_Handler, 0)
FAULT_HANDLER(MemManage_Handler, 1)
FAULT_HANDLER(BusFault_Handler, 2)
FAULT_HANDLER(UsageFault_Handler, 3)
