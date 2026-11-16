/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.11 Lab 13: Health Monitor with Crash Logging > 13.11.2 Complete Implementation
 * Verification: changed since the reference run - re-run tools/run_suite.py
 * SPDX-License-Identifier: MIT
 */
/* lab13_health_monitor.c
 * Target: STM32F407 Discovery, FreeRTOS
 * Build:  Include FreeRTOS source, CMSIS, and STM32F4 HAL (LL preferred)
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>

/* ---- UART2 polling output (PA2, 115200 baud) ---- */
static void uart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER   |= (2U << (2 * 2));     /* PA2 = AF mode         */
    GPIOA->AFR[0]  |= (7U << (2 * 4));     /* PA2 = AF7 (USART2)   */
    USART2->BRR = 0x16D;                    /* 115200 @ 42 MHz APB1  */
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

static void uart_puts(const char *s)
{
    while (*s) {
        while (!(USART2->SR & USART_SR_TXE)) {}
        USART2->DR = *s++;
    }
}

static void uart_put_hex(uint32_t val)
{
    char buf[12];
    snprintf(buf, sizeof(buf), "0x%08lX", (unsigned long)val);
    uart_puts(buf);
}

/* ---- Backup SRAM crash log ---- */
#define CRASH_LOG_BASE  ((volatile uint32_t *)BKPSRAM_BASE)
#define CRASH_MAGIC     0xC0A5B10C
#define FAULT_CTR_OFFSET 8      /* Word offset for fault counter */
#define FAULT_CTR_VALID_OFFSET 9
#define FAULT_CTR_MAGIC 0xB007C0DE

static void bkpsram_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR      |= PWR_CR_DBP;
    PWR->CSR     |= PWR_CSR_BRE;       /* Retain BKPSRAM on VBAT    */
    for (uint32_t t = 0; t < 100000U && !(PWR->CSR & PWR_CSR_BRR); t++) {}
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;
}

static void crash_log_write(uint32_t pc, uint32_t lr,
                            uint32_t cfsr, uint32_t extra)
{
    CRASH_LOG_BASE[0] = CRASH_MAGIC;
    CRASH_LOG_BASE[1] = pc;
    CRASH_LOG_BASE[2] = lr;
    CRASH_LOG_BASE[3] = cfsr;
    CRASH_LOG_BASE[4] = extra;
    CRASH_LOG_BASE[5] = xTaskGetTickCount();
}

static void crash_log_print(void)
{
    if (CRASH_LOG_BASE[0] == CRASH_MAGIC) {
        uart_puts("=== CRASH LOG FROM PREVIOUS RUN ===\r\n");
        uart_puts("  PC:    "); uart_put_hex(CRASH_LOG_BASE[1]);
        uart_puts("\r\n  LR:    "); uart_put_hex(CRASH_LOG_BASE[2]);
        uart_puts("\r\n  CFSR:  "); uart_put_hex(CRASH_LOG_BASE[3]);
        uart_puts("\r\n  Extra: "); uart_put_hex(CRASH_LOG_BASE[4]);
        uart_puts("\r\n  Tick:  "); uart_put_hex(CRASH_LOG_BASE[5]);
        uart_puts("\r\n");
        CRASH_LOG_BASE[0] = 0;         /* Clear after printing */
    } else {
        uart_puts("No crash log found.\r\n");
    }
}

/* ---- Reset cause decode ---- */
static void print_reset_cause(void)
{
    uint32_t csr = RCC->CSR;
    RCC->CSR |= RCC_CSR_RMVF;

    uart_puts("Reset cause: ");
    if (csr & RCC_CSR_IWDGRSTF)  uart_puts("[IWDG] ");
    if (csr & RCC_CSR_WWDGRSTF)  uart_puts("[WWDG] ");
    if (csr & RCC_CSR_SFTRSTF)   uart_puts("[SOFTWARE] ");
    if (csr & RCC_CSR_PORRSTF)   uart_puts("[POWER-ON] ");
    if (csr & RCC_CSR_PINRSTF)   uart_puts("[NRST PIN] ");
    if (csr & RCC_CSR_BORRSTF)   uart_puts("[BROWN-OUT] ");
    if (csr & RCC_CSR_LPWRRSTF)  uart_puts("[LOW-POWER] ");
    uart_puts("\r\n");
}

/* ---- Persistent fault counter ---- */
#define MAX_REBOOTS 5

static void check_reboot_limit(void)
{
    volatile uint32_t *counter = &CRASH_LOG_BASE[FAULT_CTR_OFFSET];
    if (CRASH_LOG_BASE[FAULT_CTR_VALID_OFFSET] != FAULT_CTR_MAGIC) {
        *counter = 0;                  /* First boot: random SRAM */
        CRASH_LOG_BASE[FAULT_CTR_VALID_OFFSET] = FAULT_CTR_MAGIC;
    }
    if (*counter >= MAX_REBOOTS) {
        uart_puts("REBOOT LIMIT REACHED — entering safe state\r\n");
        /* In a real system: drive outputs safe, halt. */
        __disable_irq();
        for (;;) { __NOP(); }
    }
    (*counter)++;
}

static void clear_reboot_counter(void)
{
    CRASH_LOG_BASE[FAULT_CTR_OFFSET] = 0;
}

/* ---- IWDG ---- */
static void iwdg_init(void)
{
    IWDG->KR  = 0x5555;
    IWDG->PR  = 4;                     /* Prescaler /64             */
    IWDG->RLR = 1000;                  /* ~2 s timeout @ 32 kHz LSI */
    while (IWDG->SR) {}
    IWDG->KR  = 0xAAAA;
    IWDG->KR  = 0xCCCC;                /* Start — cannot be stopped */
}

static void iwdg_refresh(void)
{
    IWDG->KR = 0xAAAA;
}

/* ---- Health monitor ---- */
#define NUM_TASKS           3
#define ALL_TASKS_MASK      ((1U << NUM_TASKS) - 1)
#define MONITOR_PERIOD_MS   500
#define MISS_LIMIT          4
#define STABLE_PERIODS      20         /* 10 s healthy clears counter */

static volatile uint32_t health_bits = 0;
static uint32_t miss_counts[NUM_TASKS] = {0};

static void health_checkin(uint32_t bit)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    health_bits |= bit;
    __set_PRIMASK(primask);
}

static void monitor_task(void *pv)
{
    TickType_t last = xTaskGetTickCount();
    uint32_t healthy_periods = 0;
    for (;;) {
        vTaskDelayUntil(&last, pdMS_TO_TICKS(MONITOR_PERIOD_MS));

        /* Snapshot and clear */
        __disable_irq();
        uint32_t snap = health_bits;
        health_bits = 0;
        __enable_irq();

        uint32_t missed = ALL_TASKS_MASK & ~snap;
        uint8_t all_ok = 1;

        for (int i = 0; i < NUM_TASKS; i++) {
            if (missed & (1U << i)) {
                miss_counts[i]++;
                uart_puts("WARN: task "); uart_put_hex(i);
                uart_puts(" missed ("); uart_put_hex(miss_counts[i]);
                uart_puts(")\r\n");
                all_ok = 0;

                if (miss_counts[i] >= MISS_LIMIT) {
                    uart_puts("FATAL: escalation limit\r\n");
                    crash_log_write(0, 0, missed, miss_counts[i]);
                    NVIC_SystemReset();
                }
            } else {
                miss_counts[i] = 0;
            }
        }

        if (all_ok) {
            if (healthy_periods < STABLE_PERIODS &&
                ++healthy_periods == STABLE_PERIODS) {
                clear_reboot_counter();    /* Stable operation reached */
            }
        } else {
            healthy_periods = 0;
        }

        /* Keep feeding while escalation is in progress; the IWDG fires
           only if this task stops running. */
        iwdg_refresh();
    }
}

/* ---- Application tasks ---- */
static void control_task(void *pv)
{
    for (;;) {
        health_checkin(1U << 0);
        /* Simulate 10 ms control loop */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void comms_task(void *pv)
{
    uint32_t cycle = 0;
    for (;;) {
        health_checkin(1U << 1);

        /* After 20 cycles (~2 s), simulate a fault: infinite loop */
        cycle++;
        if (cycle > 20) {
            uart_puts("comms_task: simulating hang...\r\n");
            for (;;) { /* stuck */ }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void logging_task(void *pv)
{
    for (;;) {
        health_checkin(1U << 2);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

/* ---- Main ---- */
int main(void)
{
    SystemInit();
    uart_init();
    bkpsram_init();

    uart_puts("\r\n=== Lab 13: Health Monitor ===\r\n");
    print_reset_cause();
    crash_log_print();
    check_reboot_limit();

    iwdg_init();

    xTaskCreate(control_task,  "Ctrl",  256, NULL, 3, NULL);
    xTaskCreate(comms_task,    "Comms", 256, NULL, 2, NULL);
    xTaskCreate(logging_task,  "Log",   256, NULL, 1, NULL);
    xTaskCreate(monitor_task,  "Mon",   512, NULL, 4, NULL);

    vTaskStartScheduler();
    for (;;) {}
}
