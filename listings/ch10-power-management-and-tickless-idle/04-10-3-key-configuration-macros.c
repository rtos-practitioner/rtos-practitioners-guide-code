/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 10: Power Management and Tickless Idle
 * 10.3 FreeRTOS Tickless Idle > Key Configuration Macros
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* The port passes a TickType_t lvalue, so pass its address */
#define configPRE_SLEEP_PROCESSING(xIdleTicks)   PreSleepProcessing(&(xIdleTicks))
#define configPOST_SLEEP_PROCESSING(xIdleTicks)  PostSleepProcessing(&(xIdleTicks))

void PreSleepProcessing(TickType_t *xIdleTicks)
{
    /* Disable peripheral clocks to reduce Sleep-mode current */
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_USART2_CLK_DISABLE();
}

void PostSleepProcessing(TickType_t *xIdleTicks)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
}
