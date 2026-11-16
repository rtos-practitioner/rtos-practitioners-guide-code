# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Chapter 14: Testing Real-Time Systems
# 14.7 Code Coverage for Embedded > Coverage on Host Build
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
# Compile with coverage flags
gcc -fprofile-arcs -ftest-coverage -o test_filter \
    test_moving_average.c moving_average.c -lunity

# Run the tests
./test_filter

# Generate coverage report
gcov moving_average.c
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
