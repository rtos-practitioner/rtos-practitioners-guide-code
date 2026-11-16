/* harness.c - entry point shared by every wrapped book listing.
 *
 * A generated unit provides (all optional / weak):
 *   int  book_main(void)   - the listing's own main(), renamed by the generator
 *   void unit_setup(void)  - pre-scheduler setup (create stub objects, spawn tasks / runner)
 * The harness arms a virtual-time deadline (TIM3 IRQ, highest priority) so a
 * unit that never returns is stopped deterministically and reported as a pass
 * (it ran for its whole window without faulting).
 */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "unit_prelude.h"

#ifndef UNIT_NAME
#define UNIT_NAME "unnamed"
#endif
#ifndef HARNESS_DEADLINE_TIM
#define HARNESS_DEADLINE_TIM 3   /* generator overrides if the listing uses TIM3 */
#endif
#ifndef UNIT_RUN_MS
#define UNIT_RUN_MS 1000
#endif

extern int  book_main(void)  __attribute__((weak));
extern void unit_setup(void) __attribute__((weak));

static void runner_task(void *arg)
{
    void (*fn)(void) = (void (*)(void))arg;
    fn();
    printf("[HARNESS] unit runner finished (code under test returned normally)\r\n");
    for (;;) { vTaskDelay(portMAX_DELAY); }
}

/* Statically allocated so that heap_5 listings can call vPortDefineHeapRegions() themselves. */
void harness_spawn_runner(void (*fn)(void), uint32_t stack_words, UBaseType_t prio)
{
    static StaticTask_t tcb;
    static StackType_t  stack[1536];
    (void)stack_words;
    if (xTaskCreateStatic(runner_task, "UnitRun", sizeof(stack) / sizeof(stack[0]), (void *)fn, prio, stack, &tcb) == NULL) {
        printf("[HARNESS] cannot create runner task\r\n");
        board_exit(BOARD_EXIT_MALLOC);
    }
}

/* Entry adapter for tasks spawned from listings that define task bodies. A body that returns
 * (illegal in FreeRTOS) is reported, then deleted, instead of tripping prvTaskExitError. */
void harness_task_adapter(void *arg)
{
    void (*fn)(void *) = (void (*)(void *))arg;
    fn(NULL);
    printf("[HARNESS] task '%s' returned from its body (FreeRTOS tasks must not return)\r\n",
           pcTaskGetName(NULL));
    vTaskDelete(NULL);
}

void harness_noop_timer_cb(TimerHandle_t t) { (void)t; }

/* Runs in an ISR above every FreeRTOS-managed priority: it must not use printf (the listing may have
 * retargeted _write) nor any FreeRTOS API that is not a plain read. */
void harness_deadline(uint32_t interrupted_pc)
{
    board_puts("[HARNESS] window of "); board_put_dec(UNIT_RUN_MS);
    board_puts(" ms elapsed: tasks="); board_put_dec(uxTaskGetNumberOfTasks());
    board_puts(" free_heap="); board_put_dec((uint32_t)xPortGetFreeHeapSize());
    board_puts(" min_free_heap="); board_put_dec((uint32_t)xPortGetMinimumEverFreeHeapSize());
    board_puts(" tick="); board_put_dec((uint32_t)xTaskGetTickCount());
    board_puts(" pc=0x"); board_put_hex(interrupted_pc);
    board_puts("\r\n[HARNESS] RESULT RAN_OK\r\n");
    board_exit(BOARD_EXIT_OK);
}

#if HARNESS_DEADLINE_TIM == 0
/* Used only when the listing under test occupies TIM2, TIM3 and TIM5: a top-priority task ends the run. */
static void deadline_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(UNIT_RUN_MS));
    harness_deadline(0);
}
#endif

int main(void)
{
    board_init();
    board_puts("[HARNESS] unit=" UNIT_NAME " start (window "); board_put_dec(UNIT_RUN_MS); board_puts(" ms)\r\n");
#if HARNESS_DEADLINE_TIM == 0
    {
        static StaticTask_t dtcb;
        static StackType_t  dstack[512];
        xTaskCreateStatic(deadline_task, "Deadline", 512, NULL, configMAX_PRIORITIES - 1, dstack, &dtcb);
    }
#else
    board_arm_deadline(UNIT_RUN_MS, HARNESS_DEADLINE_TIM);
#endif

    if (unit_setup) { unit_setup(); }
    if (book_main) {
        int rc = book_main();               /* normally never returns: it starts the scheduler */
        printf("[HARNESS] book main() returned %d\r\n", rc);
    }
    vTaskStartScheduler();
    printf("[HARNESS] vTaskStartScheduler returned (insufficient heap?)\r\n");
    board_exit(BOARD_EXIT_SCHED_FAIL);
}
