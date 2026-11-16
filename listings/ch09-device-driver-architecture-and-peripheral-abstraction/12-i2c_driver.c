/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.5 I2C Driver Case Study > 9.5.2 Error Handling and Bus Recovery
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* i2c_driver.c (excerpts) */
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

static I2C_HandleTypeDef   hi2c1;
static SemaphoreHandle_t   i2c_mutex;

static void i2c_bus_recovery(void)
{
    /* Reconfigure SCL as GPIO output, SDA as GPIO input */
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = GPIO_PIN_6;  /* PB6 = SCL */
    gpio.Mode  = GPIO_MODE_OUTPUT_OD;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpio);

    /* Toggle SCL up to 9 times to free SDA */
    for (int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_Delay(1);

        /* Check if SDA is released (PB7) */
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_SET) {
            break;
        }
    }

    /* Generate STOP condition: SDA low-to-high while SCL is high */
    gpio.Pin = GPIO_PIN_7;  /* SDA */
    gpio.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(GPIOB, &gpio);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);

    /* Re-initialize I2C peripheral */
    HAL_I2C_Init(&hi2c1);
}
