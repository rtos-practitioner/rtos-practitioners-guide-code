/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.3 The Fake HAL Pattern (Expanded) > Code Example: Fake I2C HAL with Programmable Responses
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* fake_i2c_hal.c */
#include "hal_api.h"
#include "fake_hal_test.h"
#include <string.h>

#define MAX_TRANSACTIONS 64
#define MAX_DATA_LEN     32

typedef struct {
    uint8_t  addr;
    uint8_t  reg;
    uint8_t  data[MAX_DATA_LEN];
    uint16_t len;
    bool     is_write;
} I2CTransaction;

/* Pre-programmed responses */
static uint8_t        response_data[MAX_TRANSACTIONS][MAX_DATA_LEN];
static HAL_StatusTypeDef response_status[MAX_TRANSACTIONS];
static int            response_count;
static int            response_index;

/* Recorded transactions */
static I2CTransaction recorded[MAX_TRANSACTIONS];
static int            recorded_count;

void fake_i2c_reset(void) {
    memset(response_data, 0, sizeof(response_data));
    memset(recorded, 0, sizeof(recorded));
    response_count = 0;
    response_index = 0;
    recorded_count = 0;
}

void fake_i2c_enqueue_response(const uint8_t *data, uint16_t len,
                                HAL_StatusTypeDef status) {
    memcpy(response_data[response_count], data, len);
    response_status[response_count] = status;
    response_count++;
}

int fake_i2c_get_transaction_count(void) {
    return recorded_count;
}

const I2CTransaction *fake_i2c_get_transaction(int index) {
    return &recorded[index];
}

/* HAL API implementation */
HAL_StatusTypeDef hal_i2c_read(uint8_t addr, uint8_t reg,
                                uint8_t *buf, uint16_t len) {
    /* Record the transaction */
    recorded[recorded_count].addr     = addr;
    recorded[recorded_count].reg      = reg;
    recorded[recorded_count].len      = len;
    recorded[recorded_count].is_write = false;
    recorded_count++;

    /* Return pre-programmed response */
    if (response_index < response_count) {
        memcpy(buf, response_data[response_index], len);
        return response_status[response_index++];
    }
    return HAL_ERROR;  /* no response programmed */
}

HAL_StatusTypeDef hal_i2c_write(uint8_t addr, uint8_t reg,
                                 const uint8_t *data, uint16_t len) {
    recorded[recorded_count].addr     = addr;
    recorded[recorded_count].reg      = reg;
    memcpy(recorded[recorded_count].data, data, len);
    recorded[recorded_count].len      = len;
    recorded[recorded_count].is_write = true;
    recorded_count++;

    if (response_index < response_count) {
        return response_status[response_index++];
    }
    return HAL_OK;
}
