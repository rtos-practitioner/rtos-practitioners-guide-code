/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.4 SPI Driver Case Study > 9.4.3 Transaction-Based API
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* spi_driver.c */
#include "spi_driver.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <string.h>

static SPI_HandleTypeDef   hspi1;
static SemaphoreHandle_t   spi_mutex;

int spi_init(void)
{
    spi_mutex = xSemaphoreCreateMutex();
    if (!spi_mutex) return -1;

    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* PA5 = SCK, PA6 = MISO, PA7 = MOSI (AF5) */
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin       = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpio.Mode      = GPIO_MODE_AF_PP;
    gpio.Pull      = GPIO_NOPULL;
    gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpio);

    hspi1.Instance               = SPI1;
    hspi1.Init.Mode              = SPI_MODE_MASTER;
    hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi1.Init.NSS               = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    HAL_SPI_Init(&hspi1);

    return 0;
}

static void cs_assert(const spi_device_t *dev)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)dev->cs_port,
                      dev->cs_pin, GPIO_PIN_RESET);
}

static void cs_deassert(const spi_device_t *dev)
{
    HAL_GPIO_WritePin((GPIO_TypeDef *)dev->cs_port,
                      dev->cs_pin, GPIO_PIN_SET);
}

/* Reconfigure CPOL/CPHA if needed for this device */
static void spi_configure(const spi_device_t *dev)
{
    uint32_t desired_cpol = dev->cpol ? SPI_POLARITY_HIGH
                                      : SPI_POLARITY_LOW;
    uint32_t desired_cpha = dev->cpha ? SPI_PHASE_2EDGE
                                      : SPI_PHASE_1EDGE;
    if (hspi1.Init.CLKPolarity != desired_cpol ||
        hspi1.Init.CLKPhase   != desired_cpha) {
        hspi1.Init.CLKPolarity = desired_cpol;
        hspi1.Init.CLKPhase   = desired_cpha;
        HAL_SPI_Init(&hspi1);
    }
}

int spi_transfer(const spi_device_t *dev, const uint8_t *tx_buf,
                 uint8_t *rx_buf, size_t len, uint32_t timeout_ms)
{
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms);

    if (xSemaphoreTake(spi_mutex, ticks) != pdTRUE) {
        return -1;  /* Bus busy */
    }

    spi_configure(dev);

    cs_assert(dev);

    HAL_StatusTypeDef status;
    if (tx_buf && rx_buf) {
        status = HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)tx_buf,
                                          rx_buf, len, timeout_ms);
    } else if (tx_buf) {
        status = HAL_SPI_Transmit(&hspi1, (uint8_t *)tx_buf,
                                   len, timeout_ms);
    } else {
        /* For read-only, transmit len dummy 0xFF bytes from rx_buf */
        memset(rx_buf, 0xFF, len);
        status = HAL_SPI_TransmitReceive(&hspi1, rx_buf,
                                          rx_buf, len, timeout_ms);
    }

    cs_deassert(dev);

    xSemaphoreGive(spi_mutex);

    return (status == HAL_OK) ? (int)len : -1;
}

int spi_write_then_read(const spi_device_t *dev,
                        const uint8_t *tx_buf, size_t tx_len,
                        uint8_t *rx_buf, size_t rx_len,
                        uint32_t timeout_ms)
{
    TickType_t ticks = pdMS_TO_TICKS(timeout_ms);

    if (xSemaphoreTake(spi_mutex, ticks) != pdTRUE) {
        return -1;  /* Bus busy */
    }

    spi_configure(dev);

    cs_assert(dev);             /* CS stays low for both phases */

    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, (uint8_t *)tx_buf,
                                                tx_len, timeout_ms);
    if (status == HAL_OK) {
        memset(rx_buf, 0xFF, rx_len);
        status = HAL_SPI_TransmitReceive(&hspi1, rx_buf,
                                          rx_buf, rx_len, timeout_ms);
    }

    cs_deassert(dev);

    xSemaphoreGive(spi_mutex);

    return (status == HAL_OK) ? (int)rx_len : -1;
}
