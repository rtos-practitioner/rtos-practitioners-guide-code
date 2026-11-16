/* unit_prelude.h - what every wrapped listing can assume is already included.
 * Book listings routinely omit includes ("stm32f4xx_hal.h", <stdio.h>, ...);
 * this header supplies the union of everything a typical STM32 project has. */
#ifndef UNIT_PRELUDE_H
#define UNIT_PRELUDE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <inttypes.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "message_buffer.h"

#include "stm32f4xx_hal.h"
#include "board.h"

/* harness API used by generated glue */
void harness_spawn_runner(void (*fn)(void), uint32_t stack_words, UBaseType_t prio);
void harness_noop_timer_cb(TimerHandle_t t);
void harness_task_adapter(void *fn);   /* use as task entry with the real body as pvParameters */

#endif
