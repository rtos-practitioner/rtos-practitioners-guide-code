/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 5: Semaphores, Mutexes, and Resource Sharing
 * 5.1 The Critical Section Problem > 5.1.1 A Concrete Race: Read-Modify-Write
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
LDR   R0, [R1]      ; Read current value
ADDS  R0, R0, #1    ; Modify (increment)
STR   R0, [R1]      ; Write back
