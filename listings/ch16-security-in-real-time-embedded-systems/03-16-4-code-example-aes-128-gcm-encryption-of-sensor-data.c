/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 16: Security in Real-Time Embedded Systems
 * 16.4 Cryptographic Operations > Code Example: AES-128-GCM Encryption of Sensor Data
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
#include "stm32f4xx_hal.h"
#include "mbedtls/gcm.h"
#include "mbedtls/platform_util.h"
#include <string.h>

#define GCM_NONCE_LEN  12U   /* 96-bit nonce */
#define GCM_TAG_LEN    16U
#define GCM_OVERHEAD   (GCM_NONCE_LEN + GCM_TAG_LEN)

static RNG_HandleTypeDef hrng = { .Instance = RNG };
static mbedtls_gcm_context gcm;  /* Use from one task, or guard with a mutex */

/* key: loaded from the secure element by the caller, which zeroizes its
 * copy (mbedtls_platform_zeroize) once this returns. */
int crypto_init(const uint8_t key[16])
{
    __HAL_RCC_RNG_CLK_ENABLE();
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        return -1;
    }
    mbedtls_gcm_init(&gcm);
    if (mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, 128) != 0) {
        return -1;
    }
    return 0;
}

/* Generate a 96-bit random nonce from the hardware RNG */
static int generate_nonce(uint8_t nonce[GCM_NONCE_LEN])
{
    for (int i = 0; i < 3; i++) {
        uint32_t rnd;
        /* Returns an error on RNG seed/clock faults (SECS/CECS) */
        if (HAL_RNG_GenerateRandomNumber(&hrng, &rnd) != HAL_OK) {
            return -1;
        }
        memcpy(&nonce[i * 4], &rnd, 4);
    }
    return 0;
}

/* Encrypt and authenticate a sensor record.
 * aad: optional data that is authenticated but not encrypted (e.g., record ID).
 * Output: [12-byte nonce][ciphertext][16-byte tag] */
int encrypt_sensor_data(const uint8_t *plaintext, uint32_t len,
                        const uint8_t *aad, uint32_t aad_len,
                        uint8_t *output, uint32_t output_size)
{
    if (output_size < GCM_OVERHEAD || len > output_size - GCM_OVERHEAD) {
        return -1;
    }

    uint8_t *nonce = output;
    uint8_t *ct    = output + GCM_NONCE_LEN;
    uint8_t *tag   = ct + len;

    if (generate_nonce(nonce) != 0) {
        return -1;
    }
    if (mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, len,
                                  nonce, GCM_NONCE_LEN, aad, aad_len,
                                  plaintext, ct, GCM_TAG_LEN, tag) != 0) {
        return -1;
    }
    return (int)(len + GCM_OVERHEAD);
}

/* Verify and decrypt a record produced by encrypt_sensor_data().
 * No plaintext is released unless the tag verifies. */
int decrypt_sensor_data(const uint8_t *input, uint32_t input_len,
                        const uint8_t *aad, uint32_t aad_len,
                        uint8_t *plaintext, uint32_t plaintext_size)
{
    if (input_len < GCM_OVERHEAD) {
        return -1;
    }
    uint32_t len = input_len - GCM_OVERHEAD;
    if (len > plaintext_size) {
        return -1;
    }

    if (mbedtls_gcm_auth_decrypt(&gcm, len, input, GCM_NONCE_LEN,
                                 aad, aad_len,
                                 input + GCM_NONCE_LEN + len, GCM_TAG_LEN,
                                 input + GCM_NONCE_LEN, plaintext) != 0) {
        mbedtls_platform_zeroize(plaintext, len);
        return -2;  /* Tampered or corrupted record */
    }
    return (int)len;
}
