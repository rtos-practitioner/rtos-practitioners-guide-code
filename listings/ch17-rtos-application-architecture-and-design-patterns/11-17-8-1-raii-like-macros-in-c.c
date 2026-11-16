/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.8 Resource Management Patterns > 17.8.1 RAII-Like Macros in C
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
#define SPI_ACQUIRE(mutex, timeout)                          \
    do {                                                     \
        if (xSemaphoreTake((mutex), (timeout)) != pdTRUE) {  \
            vLogError("SPI acquire timeout");                \
            return ERROR_TIMEOUT;                            \
        }                                                    \
    } while (0)

#define SPI_RELEASE(mutex)                                   \
    do { xSemaphoreGive((mutex)); } while (0)

ErrorCode_t eSpiWriteDisplay(const uint8_t *pucData, size_t xLen)
{
    SPI_ACQUIRE(xSpiMutex, pdMS_TO_TICKS(100));
    vSpiSelectChip(SPI_CS_DISPLAY);
    ErrorCode_t eResult = eSpiTransmit(pucData, xLen);
    vSpiDeselectChip(SPI_CS_DISPLAY);
    SPI_RELEASE(xSpiMutex);
    return eResult;
}
