#!/bin/bash
# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Chapter 14: Testing Real-Time Systems
# 14.8 Continuous Integration for Embedded > Automated Flash and Test
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
# ci_target_test.sh — called by CI runner

# Flash firmware via OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
    -c "program build/firmware.elf verify reset exit"

# Wait for boot
sleep 2

# Run tests via UART, capture output
python3 scripts/run_target_tests.py /dev/ttyACM0 115200 \
    --timeout 60 --output results/target_tests.xml

# Parse results
python3 scripts/parse_junit.py results/target_tests.xml
