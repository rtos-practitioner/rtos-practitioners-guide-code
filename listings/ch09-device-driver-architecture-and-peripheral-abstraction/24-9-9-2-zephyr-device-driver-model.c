/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.9 QNX and Zephyr Portability Notes > 9.9.2 Zephyr: Device Driver Model
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* Zephyr: devicetree-based UART access */
const struct device *uart_dev =
    DEVICE_DT_GET(DT_NODELABEL(usart2));

if (!device_is_ready(uart_dev)) {
    LOG_ERR("UART device not ready");
    return -1;
}

/* Zephyr UART API */
uart_irq_callback_set(uart_dev, uart_isr_callback);
uart_irq_rx_enable(uart_dev);
