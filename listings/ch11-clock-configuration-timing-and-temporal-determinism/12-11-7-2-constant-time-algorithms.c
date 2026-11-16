/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.7 Temporal Determinism Strategies > 11.7.2 Constant-Time Algorithms
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Non-constant time: branch on data */
if (error > threshold)
    output = max_output;
else
    output = pid_compute(error);

/* Constant time: branchless select */
int32_t saturated = (error > threshold);
output = saturated * max_output + (1 - saturated) * pid_compute(error);
