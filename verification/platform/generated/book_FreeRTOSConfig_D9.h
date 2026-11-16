#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Scheduling */
#define configUSE_PREEMPTION                     1
#define configUSE_TIME_SLICING                   1
#define configUSE_TICKLESS_IDLE                  0
#define configCPU_CLOCK_HZ                       ( 168000000UL )
#define configTICK_RATE_HZ                       ( 1000 )
#define configMAX_PRIORITIES                     ( 8 )
#define configMINIMAL_STACK_SIZE                 ( 128 )
#define configIDLE_SHOULD_YIELD                  1

/* Memory */
#define configTOTAL_HEAP_SIZE                    ( 65536 )
#define configSUPPORT_STATIC_ALLOCATION          1
#define configSUPPORT_DYNAMIC_ALLOCATION         1
#define configAPPLICATION_ALLOCATED_HEAP          0

/* Features */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_COUNTING_SEMAPHORES            1
#define configUSE_QUEUE_SETS                     0
#define configUSE_TASK_NOTIFICATIONS             1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    1

/* Software timers */
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                ( 2 )
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             ( 256 )

/* Debug */
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_TRACE_FACILITY                 1
#define configGENERATE_RUN_TIME_STATS            1
#define configUSE_STATS_FORMATTING_FUNCTIONS     1
#define configASSERT( x ) if( !( x ) ) { taskDISABLE_INTERRUPTS(); for( ;; ); }
#define configRECORD_STACK_HIGH_ADDRESS           1

/* Hook functions */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configUSE_MALLOC_FAILED_HOOK             1
#define configUSE_DAEMON_TASK_STARTUP_HOOK       0

/* ARM Cortex-M4 interrupt priorities */
#define configPRIO_BITS                          4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY          ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* SMP (single-core, all disabled) */
#define configNUMBER_OF_CORES                    1

/* Cortex-M specific definitions */
#define xPortPendSVHandler                       PendSV_Handler
#define vPortSVCHandler                          SVC_Handler

#endif /* FREERTOS_CONFIG_H */
