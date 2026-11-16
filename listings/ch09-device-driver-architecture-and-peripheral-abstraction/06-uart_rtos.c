/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.3 UART Driver Case Study > 9.3.3 Layer 3: RTOS-Aware Driver
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* uart_rtos.c -- FreeRTOS integration layer */
#include "uart_rtos.h"
#include "uart_hal.h"
#include "uart_hw.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#define UART_TX_QUEUE_LEN  256
#define UART_RX_BUF_SIZE   256

/* TX infrastructure */
static QueueHandle_t      tx_queue;
static SemaphoreHandle_t  tx_complete_sem;

/* RX circular buffer */
static volatile uint8_t   rx_buf[UART_RX_BUF_SIZE];
static volatile uint16_t  rx_head;  /* ISR writes here   */
static volatile uint16_t  rx_tail;  /* Task reads here    */
static SemaphoreHandle_t  rx_data_sem;

/* Mutex for multi-task TX serialization */
static SemaphoreHandle_t  tx_mutex;
static volatile BaseType_t tx_all_queued;  /* Task finished enqueuing */

extern UART_HandleTypeDef huart2;

int uart_rtos_init(uint32_t baudrate)
{
    tx_queue        = xQueueCreate(UART_TX_QUEUE_LEN, sizeof(uint8_t));
    tx_complete_sem = xSemaphoreCreateBinary();
    rx_data_sem     = xSemaphoreCreateCounting(UART_RX_BUF_SIZE, 0);
    tx_mutex        = xSemaphoreCreateMutex();

    if (!tx_queue || !tx_complete_sem || !rx_data_sem || !tx_mutex) {
        return -1;
    }

    rx_head = 0;
    rx_tail = 0;

    if (uart_hal_init(baudrate) != 0) {
        return -1;
    }

    /* Enable RX interrupt -- bytes will arrive asynchronously */
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

    return 0;
}

/* Called from USART2 ISR when RXNE flag is set */
void uart_rtos_rx_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint8_t byte = uart_hal_read_byte();

    uint16_t next_head = (rx_head + 1) % UART_RX_BUF_SIZE;
    if (next_head != rx_tail) {           /* Buffer not full */
        rx_buf[rx_head] = byte;
        rx_head = next_head;
        xSemaphoreGiveFromISR(rx_data_sem, &xHigherPriorityTaskWoken);
    }
    /* else: overflow -- byte is dropped; production code should count */

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Called from USART2 ISR when TXE flag is set */
void uart_rtos_tx_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint8_t byte;

    if (xQueueReceiveFromISR(tx_queue, &byte,
                              &xHigherPriorityTaskWoken) == pdTRUE) {
        uart_hal_write_byte(byte);
    } else {
        /* Queue empty -- stop TXE, wait for last byte to leave the wire */
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Called from USART2 ISR when TC flag is set */
void uart_rtos_tc_isr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
    if (tx_all_queued &&
        uxQueueMessagesWaitingFromISR(tx_queue) == 0) {
        xSemaphoreGiveFromISR(tx_complete_sem, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Global IRQ handler -- routes to appropriate sub-handler */
void USART2_IRQHandler(void)
{
    uint32_t sr = huart2.Instance->SR;

    if (sr & UART_SR_RXNE) {
        uart_rtos_rx_isr();
    }
    if ((sr & UART_SR_TXE) &&
        (huart2.Instance->CR1 & UART_CR1_TXEIE)) {
        uart_rtos_tx_isr();
    }
    if ((sr & UART_SR_TC) &&
        (huart2.Instance->CR1 & UART_CR1_TCIE)) {
        uart_rtos_tc_isr();
    }
}

int uart_rtos_write(const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms);

    if (xSemaphoreTake(tx_mutex, ticks) != pdTRUE) {
        return -1;  /* Another task holds the TX path */
    }

    (void)xSemaphoreTake(tx_complete_sem, 0);  /* Discard stale signal */
    tx_all_queued = pdFALSE;

    /* Enqueue bytes; TXE is enabled after each one so the ISR drains
     * the queue concurrently and len may exceed UART_TX_QUEUE_LEN */
    for (size_t i = 0; i < len; i++) {
        if (xQueueSend(tx_queue, &data[i], ticks) != pdTRUE) {
            xSemaphoreGive(tx_mutex);
            return (int)i;  /* Partial write */
        }
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
    }

    tx_all_queued = pdTRUE;
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);  /* Ensure TC path runs */

    /* Wait for TX to drain */
    xSemaphoreTake(tx_complete_sem, ticks);

    xSemaphoreGive(tx_mutex);
    return (int)len;
}

int uart_rtos_read(uint8_t *buf, size_t len, uint32_t timeout_ms)
{
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms);

    for (size_t i = 0; i < len; i++) {
        if (xSemaphoreTake(rx_data_sem, ticks) != pdTRUE) {
            return (int)i;  /* Partial read (timeout) */
        }
        buf[i] = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % UART_RX_BUF_SIZE;
    }
    return (int)len;
}
