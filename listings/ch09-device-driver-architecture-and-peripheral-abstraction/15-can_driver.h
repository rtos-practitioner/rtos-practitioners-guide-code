/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 9: Device Driver Architecture and Peripheral Abstraction
 * 9.6 CAN Bus Driver > 9.6.2 FreeRTOS Integration
 * Verification: compiled, linked and booted (declarations only)
 * SPDX-License-Identifier: MIT
 */
/* can_driver.h */
#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

typedef struct {
    uint32_t id;          /* CAN identifier                    */
    uint8_t  id_type;     /* 0 = standard (11-bit), 1 = ext    */
    uint8_t  dlc;         /* Data length code (0-8)             */
    uint8_t  data[8];     /* Payload                            */
} can_msg_t;

int  can_init(uint32_t bitrate);
int  can_send(const can_msg_t *msg, uint32_t timeout_ms);
int  can_receive(can_msg_t *msg, uint32_t timeout_ms);
int  can_add_filter(uint32_t id, uint32_t mask);

#endif /* CAN_DRIVER_H */
