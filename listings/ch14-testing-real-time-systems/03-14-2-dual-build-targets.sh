# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Chapter 14: Testing Real-Time Systems
# 14.2 Unit Testing on Host > Dual Build Targets
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
mkdir build_fw && cd build_fw
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-none-eabi.cmake ..
cmake --build .
