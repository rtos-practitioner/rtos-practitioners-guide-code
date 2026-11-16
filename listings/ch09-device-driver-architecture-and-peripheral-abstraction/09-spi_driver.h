/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.4 SPI Driver Case Study > 9.4.2 Bus Sharing with Mutex
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* spi_driver.h */
#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void        *hw_handle;    /* Opaque: SPI_HandleTypeDef*     */
    uint16_t     cs_pin;       /* GPIO pin for chip select       */
    void        *cs_port;      /* GPIO port for chip select      */
    uint8_t      cpol;         /* Clock polarity (0 or 1)        */
    uint8_t      cpha;         /* Clock phase (0 or 1)           */
} spi_device_t;

int spi_init(void);
int spi_transfer(const spi_device_t *dev, const uint8_t *tx_buf,
                 uint8_t *rx_buf, size_t len, uint32_t timeout_ms);
/* Command phase then data phase under a single CS assertion */
int spi_write_then_read(const spi_device_t *dev,
                        const uint8_t *tx_buf, size_t tx_len,
                        uint8_t *rx_buf, size_t rx_len,
                        uint32_t timeout_ms);

#endif /* SPI_DRIVER_H */
