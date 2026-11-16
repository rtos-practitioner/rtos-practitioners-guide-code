/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 16: Security in Real-Time Embedded Systems
 * 16.10 Lab 16: Secure Boot and Authenticated Commands > Part B: HMAC-Authenticated UART Command Protocol
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "hmac_sha256.h"
#include "uart_driver.h"

#define CMD_FRAME_LEN   20
#define HMAC_TRUNC_LEN   8
#define HMAC_KEY_LEN    32

static const uint8_t cmd_hmac_key[HMAC_KEY_LEN] = {
    /* In production, load from OTP or secure element */
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
};

static uint32_t last_rx_seq = 0;

typedef struct {
    uint32_t seq_num;
    uint8_t  cmd_code;
    uint8_t  payload[7];
} command_t;

/* Verify and extract a command from a raw frame.
 * Returns 0 on success, negative on failure. */
int verify_command(const uint8_t *frame, command_t *cmd_out)
{
    uint32_t t_start = DWT->CYCCNT;  /* Start cycle counter */

    /* Compute expected HMAC over first 12 bytes */
    uint8_t expected_hmac[32];
    hmac_sha256(cmd_hmac_key, HMAC_KEY_LEN,
                frame, 12, expected_hmac);

    /* Constant-time comparison of truncated tag */
    uint8_t diff = 0;
    for (int i = 0; i < HMAC_TRUNC_LEN; i++) {
        diff |= frame[12 + i] ^ expected_hmac[i];
    }

    uint32_t t_hmac = DWT->CYCCNT - t_start;

    if (diff != 0) {
        uart_printf("AUTH FAIL (HMAC verify: %lu cycles)\r\n", t_hmac);
        return -1;
    }

    /* Extract sequence number (little-endian) */
    uint32_t seq = (uint32_t)frame[0]       |
                   (uint32_t)frame[1] << 8  |
                   (uint32_t)frame[2] << 16 |
                   (uint32_t)frame[3] << 24;

    /* Anti-replay check */
    if (seq <= last_rx_seq) {
        uart_printf("REPLAY REJECTED (seq=%lu, last=%lu)\r\n",
                    seq, last_rx_seq);
        return -2;
    }
    last_rx_seq = seq;

    /* Extract command fields */
    cmd_out->seq_num  = seq;
    cmd_out->cmd_code = frame[4];
    memcpy(cmd_out->payload, &frame[5], 7);

    uart_printf("CMD OK: code=0x%02X seq=%lu (HMAC: %lu cycles, "
                "%.1f us @ 168MHz)\r\n",
                cmd_out->cmd_code, seq,
                t_hmac, (float)t_hmac / 168.0f);
    return 0;
}

/* Main command processing loop (runs as FreeRTOS task) */
void vCommandTask(void *pvParam)
{
    uint8_t frame[CMD_FRAME_LEN];
    command_t cmd;

    /* Enable DWT cycle counter for timing measurements */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    for (;;) {
        /* Block until a full 20-byte frame arrives */
        if (uart_receive(frame, CMD_FRAME_LEN,
                         pdMS_TO_TICKS(5000)) == CMD_FRAME_LEN) {
            if (verify_command(frame, &cmd) == 0) {
                execute_command(&cmd);  /* Process valid command */
            }
        }
    }
}
