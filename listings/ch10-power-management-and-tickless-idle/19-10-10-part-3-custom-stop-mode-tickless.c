/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.10 Lab 10: Tickless Idle Power Optimization > Part 3: Custom Stop-Mode Tickless
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Minimize GPIO leakage on unused ports */
__HAL_RCC_GPIOA_CLK_ENABLE();
__HAL_RCC_GPIOB_CLK_ENABLE();
__HAL_RCC_GPIOC_CLK_ENABLE();
__HAL_RCC_GPIOE_CLK_ENABLE();

GPIOA->MODER = 0xFFFFFFFF;  /* All analog */
GPIOB->MODER = 0xFFFFFFFF;
GPIOC->MODER = 0xFFFFFFFF;
GPIOE->MODER = 0xFFFFFFFF;
/* GPIOD pin 12 left as output for LED */
