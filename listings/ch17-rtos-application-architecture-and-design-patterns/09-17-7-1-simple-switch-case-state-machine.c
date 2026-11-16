/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.7 State Machine Patterns > 17.7.1 Simple Switch-Case State Machine
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
typedef enum { MOTOR_IDLE, MOTOR_STARTING, MOTOR_RUNNING,
               MOTOR_STOPPING, MOTOR_FAULT } MotorState_t;
