/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.5 Peripheral Power Management > Disabling Unused Peripheral Clocks
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Disable clocks for peripherals not currently needed */
RCC->AHB1ENR &= ~(RCC_AHB1ENR_GPIOCEN |    /* Unused GPIO port     */
                   RCC_AHB1ENR_DMA2EN);       /* DMA2 not in use      */
RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN |      /* Timer 3 not needed   */
                   RCC_APB1ENR_I2C2EN);       /* I2C2 not in use      */
