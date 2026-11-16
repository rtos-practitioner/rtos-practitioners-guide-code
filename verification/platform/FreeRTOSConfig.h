/* FreeRTOSConfig.h - harness configuration.
 *
 * Base      : the book's own Appendix D.9 "Recommended STM32F407 FreeRTOSConfig",
 *             extracted verbatim into generated/book_FreeRTOSConfig_D9.h.
 * Additions : only what is needed so every API used in the book builds, plus
 *             failure observability (assert/stack/malloc exit QEMU with a code).
 */
#ifndef QEMU_HARNESS_FREERTOS_CONFIG_H
#define QEMU_HARNESS_FREERTOS_CONFIG_H

#include "book_FreeRTOSConfig_D9.h"

#include <stdint.h>
extern uint32_t SystemCoreClock;

/* ---- Not covered by D.9, required by the V11 kernel/port ---- */
#define xPortSysTickHandler                      SysTick_Handler
#define configUSE_16_BIT_TICKS                   0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1
#define configENABLE_FPU                         1
#define configENABLE_MPU                         0
#define configENABLE_TRUSTZONE                   0
#define configRUN_FREERTOS_SECURE_ONLY           0
#define configUSE_NEWLIB_REENTRANT               0
#define configQUEUE_REGISTRY_SIZE                8
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS  3
#define configUSE_APPLICATION_TASK_TAG           1
#define configUSE_CO_ROUTINES                    0
#define configMAX_TASK_NAME_LEN                  16
#define configMAX_CO_ROUTINE_PRIORITIES          2
#define configTIMER_SERVICE_TASK_NAME            "Tmr Svc"
#define configUSE_TASK_PREEMPTION_DISABLE        0

/* ---- Deviations from D.9 (D.9 leaves these off; the chapters use them) ---- */
#undef  configUSE_QUEUE_SETS
#define configUSE_QUEUE_SETS                     1      /* Ch.4 queue sets */
#undef  configUSE_IDLE_HOOK
#define configUSE_IDLE_HOOK                      1      /* weak default does WFI */
#undef  configUSE_TICK_HOOK
#define configUSE_TICK_HOOK                      1
#undef  configUSE_DAEMON_TASK_STARTUP_HOOK
#define configUSE_DAEMON_TASK_STARTUP_HOOK       0

/* ---- INCLUDE_ switches: everything on ---- */
#define INCLUDE_vTaskPrioritySet                 1
#define INCLUDE_uxTaskPriorityGet                1
#define INCLUDE_vTaskDelete                      1
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_xResumeFromISR                   1
#define INCLUDE_vTaskDelayUntil                  1
#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskGetSchedulerState           1
#define INCLUDE_xTaskGetCurrentTaskHandle        1
#define INCLUDE_uxTaskGetStackHighWaterMark      1
#define INCLUDE_uxTaskGetStackHighWaterMark2     1
#define INCLUDE_xTaskGetIdleTaskHandle           1
#define INCLUDE_eTaskGetState                    1
#define INCLUDE_xEventGroupSetBitFromISR         1
#define INCLUDE_xTimerPendFunctionCall           1
#define INCLUDE_xTaskAbortDelay                  1
#define INCLUDE_xTaskGetHandle                   1
#define INCLUDE_xTaskResumeFromISR               1
#define INCLUDE_xSemaphoreGetMutexHolder         1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle   1

/* ---- Run-time statistics (D.9 enables them but defines no timer) ---- */
void     vConfigureTimerForRunTimeStats(void);
uint32_t ulGetRunTimeCounterValue(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE()         ulGetRunTimeCounterValue()

/* ---- Observable failures: print + exit QEMU with a distinct code ---- */
void board_assert_failed(const char *file, int line);
#undef  configASSERT
#define configASSERT(x) do { if (!(x)) { board_assert_failed(__FILE__, __LINE__); } } while (0)

#endif /* QEMU_HARNESS_FREERTOS_CONFIG_H */
