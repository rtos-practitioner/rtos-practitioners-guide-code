/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.5 System Testing on Target > Test Harness Design
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
typedef struct {
    const char *name;
    bool (*func)(void);
} TargetTest;

static const TargetTest target_tests[] = {
    {"test_adc_reads_within_range",  test_adc_range},
    {"test_uart_loopback",           test_uart_loopback},
    {"test_spi_flash_write_read",    test_spi_flash},
    {"test_gpio_toggle_timing",      test_gpio_timing},
};
