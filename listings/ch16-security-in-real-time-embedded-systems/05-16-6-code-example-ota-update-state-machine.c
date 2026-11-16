/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 16: Security in Real-Time Embedded Systems
 * 16.6 Over-the-Air (OTA) Firmware Updates > Code Example: OTA Update State Machine
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "image_verify.h"   /* verify_image(), image_header_t, IMG_HDR_SIZE,
                               antirollback_advance() (Section 16.2)      */
#include "flash_driver.h"
#include "ota_metadata.h"   /* metadata_load()/metadata_store(): sectors 2-3,
                               two CRC-protected copies                    */

#define SLOT_A_SIZE   (448U * 1024U)   /* Sectors 4-7  */
#define SLOT_B_BASE   0x08080000U      /* Sectors 8-11 */
#define SLOT_B_SIZE   (512U * 1024U)

typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_SWAPPING,
    OTA_STATE_TESTING,
    OTA_STATE_CONFIRMED,
    OTA_STATE_ROLLBACK,
    OTA_STATE_ERROR
} ota_state_t;

/* Persistent state, stored in the metadata sectors */
typedef struct {
    uint32_t current_version;   /* Confirmed image                     */
    uint32_t pending_version;   /* Installed, not yet confirmed; 0 = none */
    uint32_t swap_requested;    /* Bootloader: install slot B          */
    uint32_t test_boot;         /* New image is on trial               */
} ota_metadata_t;

typedef struct {
    ota_state_t state;
    uint32_t    expected_size;
    uint32_t    received_size;
    uint32_t    expected_version;
} ota_context_t;

static ota_context_t  ota;
static ota_metadata_t meta;
static bool           meta_valid;

/* Call at startup, before ota_begin() or ota_confirm() */
int ota_init(void)
{
    if (metadata_load(&meta) != 0) {
        ota.state = OTA_STATE_ERROR;
        return -1;  /* No valid metadata copy: refuse updates */
    }
    meta_valid = true;
    ota.expected_version = meta.pending_version;
    ota.state = meta.test_boot ? OTA_STATE_TESTING : OTA_STATE_IDLE;
    return 0;
}

int ota_begin(uint32_t image_size, uint32_t version)
{
    if (!meta_valid || ota.state == OTA_STATE_TESTING ||
        ota.state == OTA_STATE_SWAPPING) {
        return -1;  /* No metadata, or current image not yet confirmed */
    }
    if (version <= meta.current_version) {
        return -1;  /* Anti-rollback: reject older versions */
    }
    if (image_size <= IMG_HDR_SIZE || image_size > SLOT_A_SIZE) {
        return -2;  /* Image must fit in slot A once installed */
    }

    /* Erase staging area (slot B) */
    if (flash_erase_slot_b() != 0) {
        return -3;
    }

    ota.state            = OTA_STATE_DOWNLOADING;
    ota.expected_size    = image_size;
    ota.received_size    = 0;
    ota.expected_version = version;
    return 0;
}

int ota_write_chunk(const uint8_t *data, uint32_t len)
{
    if (ota.state != OTA_STATE_DOWNLOADING) return -1;
    if (len > ota.expected_size - ota.received_size) return -2;

    if (flash_write(SLOT_B_BASE + ota.received_size, data, len) != 0) {
        ota.state = OTA_STATE_ERROR;
        return -3;
    }
    ota.received_size += len;

    if (ota.received_size == ota.expected_size) {
        ota.state = OTA_STATE_VERIFYING;
    }
    return 0;
}

int ota_verify_and_apply(void)
{
    if (ota.state != OTA_STATE_VERIFYING) return -1;

    const image_header_t *hdr = (const image_header_t *)SLOT_B_BASE;

    /* SHA-256 + ECDSA P-256 check, identical to the bootloader's.
     * The version is inside the signed header, so it cannot be forged. */
    if (verify_image(SLOT_B_BASE, SLOT_A_SIZE) != 0 ||
        hdr->image_size + IMG_HDR_SIZE != ota.expected_size ||
        hdr->version != ota.expected_version) {
        ota.state = OTA_STATE_ERROR;
        return -2;
    }

    /* Persist the request before resetting; RAM state is lost on reset */
    meta.pending_version = ota.expected_version;
    meta.swap_requested  = 1U;
    meta.test_boot       = 1U;
    if (metadata_store(&meta) != 0) {
        ota.state = OTA_STATE_ERROR;
        return -3;
    }
    ota.state = OTA_STATE_SWAPPING;

    NVIC_SystemReset();  /* Bootloader re-verifies and installs slot B */
    return 0;  /* Never reached */
}

/* Called by the new firmware, after ota_init(), once its self-test passes */
int ota_confirm(void)
{
    if (ota.state != OTA_STATE_TESTING) return -1;

    meta.current_version = meta.pending_version;
    meta.pending_version = 0U;
    meta.test_boot       = 0U;
    if (metadata_store(&meta) != 0) {
        return -2;  /* Still unconfirmed; retry before the watchdog expires */
    }
    antirollback_advance(meta.current_version);  /* OTP counter, if used */
    ota.state = OTA_STATE_CONFIRMED;
    return 0;
}
