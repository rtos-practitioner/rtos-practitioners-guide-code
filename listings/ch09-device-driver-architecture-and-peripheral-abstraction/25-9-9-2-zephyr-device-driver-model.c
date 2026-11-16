/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.9 QNX and Zephyr Portability Notes > 9.9.2 Zephyr: Device Driver Model
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
const struct device *temp_dev =
    DEVICE_DT_GET(DT_NODELABEL(lm75));

sensor_sample_fetch(temp_dev);
struct sensor_value val;
sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
float temperature = sensor_value_to_float(&val);
