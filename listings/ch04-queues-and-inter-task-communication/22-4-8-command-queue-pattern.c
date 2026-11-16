/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.8 Design Patterns and Best Practices > Command Queue Pattern
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
typedef enum {
    CMD_SENSOR_DATA,
    CMD_USER_INPUT,
    CMD_SET_CONFIG,
    CMD_SHUTDOWN
} CommandID_t;

typedef struct {
    CommandID_t eID;
    TickType_t  xTimestamp;
    union {
        SensorReading_t xSensor;
        UserInput_t     xInput;
        ConfigParam_t   xConfig;
    } uPayload;
} Command_t;

static QueueHandle_t xCommandQueue;
