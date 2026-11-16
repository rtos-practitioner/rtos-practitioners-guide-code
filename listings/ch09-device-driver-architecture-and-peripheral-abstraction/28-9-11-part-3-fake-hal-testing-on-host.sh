# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Chapter 9: Device Driver Architecture and Peripheral Abstraction
# 9.11 Lab 9: Building a Reusable UART Driver > Part 3: Fake HAL Testing on Host
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
# Host build using fake HAL
gcc -I hal/fake -I src/drivers -I test \
    src/drivers/uart_rtos.c \
    src/drivers/uart_api.c \
    hal/fake/uart_hal.c \
    test/test_command_parser.c \
    test/unity.c \
    -o test_runner

./test_runner
