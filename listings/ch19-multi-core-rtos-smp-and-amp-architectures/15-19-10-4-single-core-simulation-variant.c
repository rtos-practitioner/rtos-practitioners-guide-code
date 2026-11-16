/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.10 Lab 19: AMP Inter-Core Communication on STM32H7 > 19.10.4 Single-Core Simulation Variant
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/*
 * Single-core AMP simulation on STM32F407
 *
 * Two tasks communicate through a shared SPSC ring buffer
 * in a dedicated RAM array, emulating inter-core communication.
 */

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

#define IPC_RING_SIZE  128
#define IPC_RING_MASK  (IPC_RING_SIZE - 1)

typedef struct {
    uint32_t timestamp;
    int16_t  value;
    uint16_t sequence;
} Sample_t;

/* Simulated shared memory region */
static volatile uint32_t write_idx = 0;
static volatile uint32_t read_idx  = 0;
static Sample_t ring_buffer[IPC_RING_SIZE];

static int sim_push(const Sample_t *s)
{
    uint32_t w = write_idx;
    uint32_t nw = (w + 1) & IPC_RING_MASK;
    if (nw == read_idx) return -1;
    __DMB();
    ring_buffer[w] = *s;
    __DMB();
    write_idx = nw;
    return 0;
}

static int sim_pop(Sample_t *s)
{
    uint32_t r = read_idx;
    if (r == write_idx) return -1;
    __DMB();
    *s = ring_buffer[r];
    __DMB();
    read_idx = (r + 1) & IPC_RING_MASK;
    return 0;
}

static TaskHandle_t xConsumerHandle;

static void vSimProducer(void *pv)
{
    uint16_t seq = 0;
    TickType_t xLast = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLast, pdMS_TO_TICKS(1));
        Sample_t s = { xTaskGetTickCount(), (int16_t)(seq * 3), seq++ };
        if (sim_push(&s) == 0) {
            xTaskNotifyGive(xConsumerHandle);
        }
    }
}

static void vSimConsumer(void *pv)
{
    Sample_t s;
    uint16_t expected = 0;
    uint32_t errors = 0;
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        while (sim_pop(&s) == 0) {
            if (s.sequence != expected) errors++;
            expected = s.sequence + 1;
        }
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    xTaskCreate(vSimProducer, "SimProd", 256, NULL, 3, NULL);
    xTaskCreate(vSimConsumer, "SimCons", 256, NULL, 2,
                &xConsumerHandle);
    vTaskStartScheduler();
    for (;;) {}
}
