# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Appendix F: Development Environment Setup Guide
# F.5 Project Template > F.5.2 Build and Flash Commands
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
# Configure (run once or when CMakeLists.txt changes)
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

# Build
cmake --build build

# Flash via OpenOCD
openocd -f openocd/stm32f4discovery.cfg \
    -c "program build/app.elf verify reset exit"
