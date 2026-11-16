/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.5 DWT Cycle Counter > 11.5.5 Code Example: Measuring Function Execution Time
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint64_t total_cycles;
    uint32_t count;
} dwt_stats_t;

void dwt_stats_init(dwt_stats_t *s) {
    s->min_cycles = UINT32_MAX;
    s->max_cycles = 0;
    s->total_cycles = 0;
    s->count = 0;
}

static inline void dwt_stats_begin(uint32_t *start) {
    *start = DWT->CYCCNT;
}

static inline void dwt_stats_end(dwt_stats_t *s, uint32_t start) {
    uint32_t elapsed = DWT->CYCCNT - start;
    if (elapsed < s->min_cycles) s->min_cycles = elapsed;
    if (elapsed > s->max_cycles) s->max_cycles = elapsed;
    s->total_cycles += elapsed;
    s->count++;
}

void dwt_stats_print(const dwt_stats_t *s) {
    float min_us = (float)s->min_cycles / (configCPU_CLOCK_HZ / 1000000UL);
    float max_us = (float)s->max_cycles / (configCPU_CLOCK_HZ / 1000000UL);
    float avg_us = (float)(s->total_cycles / s->count)
                 / (configCPU_CLOCK_HZ / 1000000UL);
    printf("  min: %lu cycles (%.2f us)\r\n", s->min_cycles, min_us);
    printf("  max: %lu cycles (%.2f us)\r\n", s->max_cycles, max_us);
    printf("  avg: %.2f us  samples: %lu\r\n", avg_us, s->count);
}

/* Usage */
dwt_stats_t fft_stats;
dwt_stats_init(&fft_stats);

for (int i = 0; i < 1000; i++) {
    uint32_t t0;
    dwt_stats_begin(&t0);
    compute_fft(buffer, 1024);       /* Function under test */
    dwt_stats_end(&fft_stats, t0);
}
dwt_stats_print(&fft_stats);
