/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 17: RTOS Application Architecture and Design Patterns
 * 17.6 Command Pattern and Protocol Handlers > 17.6.3 Serial Command Processor Task
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
void vSerialCmdTask(void *pvParam)
{
    uint8_t ucRxBuffer[64];

    for (;;) {
        /* Block until a complete frame is received */
        uint16_t usLen = usUartReceiveFrame(ucRxBuffer,
                                            sizeof(ucRxBuffer),
                                            portMAX_DELAY);
        if (usLen >= 1) {
            uint8_t ucId = ucRxBuffer[0];
            vCommandProcess(ucId, &ucRxBuffer[1], usLen - 1);
        }
    }
}
