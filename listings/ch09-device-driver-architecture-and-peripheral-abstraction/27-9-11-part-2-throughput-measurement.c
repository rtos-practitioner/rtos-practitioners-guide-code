/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.11 Lab 9: Building a Reusable UART Driver > Part 2: Throughput Measurement
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void throughput_test(void)
{
    uint8_t test_data[1024];
    for (int i = 0; i < 1024; i++) test_data[i] = (uint8_t)i;

    TickType_t start, elapsed;

    /* Mode 1: Polled -- direct register polling (Layer 1) */
    start = xTaskGetTickCount();
    for (int i = 0; i < 1024; i++) {
        uart_hw_send_byte(USART2, test_data[i]);
    }
    elapsed = xTaskGetTickCount() - start;
    /* Log: "Polled: %u ms", elapsed * portTICK_PERIOD_MS */

    /* Mode 2: Interrupt-driven -- via RTOS layer (Layer 3) */
    start = xTaskGetTickCount();
    uart_write(UART_PORT_DEBUG, test_data, 1024, 5000);
    elapsed = xTaskGetTickCount() - start;
    /* Log: "Interrupt: %u ms", elapsed * portTICK_PERIOD_MS */

    /* Mode 3: DMA -- if implemented */
    start = xTaskGetTickCount();
    uart_write_dma(UART_PORT_DEBUG, test_data, 1024, 5000);
    elapsed = xTaskGetTickCount() - start;
    /* Log: "DMA: %u ms", elapsed * portTICK_PERIOD_MS */
}
