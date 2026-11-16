/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 14: Testing Real-Time Systems
 * 14.3 The Fake HAL Pattern (Expanded) > Code Example: Fake I2C HAL with Programmable Responses
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
void test_temp_sensor_reads_correct_register(void) {
    uint8_t fake_temp[] = {0x01, 0x9A};  /* 25.625 °C */
    fake_i2c_reset();
    fake_i2c_enqueue_response(fake_temp, 2, HAL_OK);

    float temp = temp_sensor_read();

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.6f, temp);
    TEST_ASSERT_EQUAL(1, fake_i2c_get_transaction_count());
    const I2CTransaction *txn = fake_i2c_get_transaction(0);
    TEST_ASSERT_EQUAL_HEX8(TEMP_SENSOR_ADDR, txn->addr);
    TEST_ASSERT_EQUAL_HEX8(TEMP_REG, txn->reg);
}

void test_temp_sensor_handles_i2c_bus_error(void) {
    fake_i2c_reset();
    fake_i2c_enqueue_response(NULL, 0, HAL_ERROR);

    float temp = temp_sensor_read();

    TEST_ASSERT_FLOAT_IS_NAN(temp);  /* or sentinel value */
}
