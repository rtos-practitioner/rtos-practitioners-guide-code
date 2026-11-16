/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.3 SysTick Timer and RTOS Tick > 11.3.4 Tick Jitter: Causes and Measurement
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* Tick jitter monitor — add to FreeRTOS SysTick hook or override handler */
static volatile uint32_t jitter_min = UINT32_MAX;
static volatile uint32_t jitter_max = 0;
static volatile uint64_t jitter_sum = 0;
static volatile uint32_t jitter_count = 0;
static volatile uint32_t prev_cycle = 0;

void vApplicationTickHook(void) {
    uint32_t now = DWT->CYCCNT;
    if (prev_cycle != 0) {
        uint32_t delta = now - prev_cycle;       /* handles wrap correctly */
        int32_t jitter = (int32_t)delta - (int32_t)(configCPU_CLOCK_HZ
                                                     / configTICK_RATE_HZ);
        uint32_t abs_jitter = (jitter < 0) ? (uint32_t)(-jitter)
                                           : (uint32_t)(jitter);
        if (abs_jitter < jitter_min) jitter_min = abs_jitter;
        if (abs_jitter > jitter_max) jitter_max = abs_jitter;
        jitter_sum += abs_jitter;
        jitter_count++;
    }
    prev_cycle = now;
}

/* Query jitter stats (call from a low-priority diagnostic task) */
void print_jitter_stats(void) {
    float min_us = (float)jitter_min / (configCPU_CLOCK_HZ / 1000000UL);
    float max_us = (float)jitter_max / (configCPU_CLOCK_HZ / 1000000UL);
    float avg_us = (float)(jitter_sum / jitter_count)
                 / (configCPU_CLOCK_HZ / 1000000UL);
    printf("Tick jitter  min: %.2f us  max: %.2f us  avg: %.2f us\r\n",
           min_us, max_us, avg_us);
}
