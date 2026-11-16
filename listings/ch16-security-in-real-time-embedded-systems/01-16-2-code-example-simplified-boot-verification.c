/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 16: Security in Real-Time Embedded Systems
 * 16.2 Secure Boot and Chain of Trust > Code Example: Simplified Boot Verification
 * Verification: bootloader-style program (no scheduler by design)
 * SPDX-License-Identifier: MIT
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include "sha256.h"         /* Software SHA-256 (e.g., wrapping mbedtls_sha256) */
#include "ecdsa_verify.h"   /* ECDSA P-256 verify (e.g., mbedTLS or micro-ecc) */

#define APP_SLOT_ADDR       0x08010000U  /* Slot A: sectors 4-7        */
#define APP_SLOT_SIZE       (448U * 1024U)
#define IMG_HDR_SIZE        0x200U       /* Header before vector table */
#define IMG_MAGIC           0x474D4941U
#define STACK_PTR_MIN       0x20000000U
#define STACK_PTR_MAX       0x20020000U

typedef struct {
    uint32_t magic;
    uint32_t image_size;     /* Bytes following the header              */
    uint32_t version;        /* Checked against the anti-rollback counter */
    uint32_t reserved;
    uint8_t  signature[64];  /* ECDSA P-256 (r||s) over
                                SHA-256(first 16 header bytes || image)  */
} image_header_t;

/* Compiled into the bootloader, so it lives in the write-protected
 * sectors 0-1 together with the code that uses it. */
static const uint8_t vendor_public_key[64] = { /* ... */ };

extern uint32_t antirollback_min_version(void);  /* OTP-backed counter */

typedef void (*app_entry_t)(void);

int verify_image(uint32_t slot_addr, uint32_t slot_size)
{
    const image_header_t *hdr = (const image_header_t *)slot_addr;
    sha256_context_t ctx;
    uint8_t digest[32];

    if (hdr->magic != IMG_MAGIC ||
        hdr->image_size == 0U ||
        hdr->image_size > slot_size - IMG_HDR_SIZE) {
        return -1;          /* Malformed header */
    }
    if (hdr->version < antirollback_min_version()) {
        return -1;          /* Rollback attempt */
    }

    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t *)hdr, 16U);  /* magic..reserved */
    sha256_update(&ctx, (const uint8_t *)(slot_addr + IMG_HDR_SIZE),
                  hdr->image_size);
    sha256_final(&ctx, digest);

    return (ecdsa_verify_p256(vendor_public_key, digest,
                              hdr->signature) == 0) ? 0 : -1;
}

static void jump_to_application(uint32_t addr)
{
    uint32_t app_stack = *(volatile uint32_t *)addr;
    uint32_t app_reset = *(volatile uint32_t *)(addr + 4U);

    /* Sanity check: stack pointer in SRAM, reset vector inside the slot */
    if (app_stack < STACK_PTR_MIN || app_stack > STACK_PTR_MAX ||
        app_reset < addr || app_reset >= APP_SLOT_ADDR + APP_SLOT_SIZE) {
        /* Invalid image --- halt */
        while (1) { __WFI(); }
    }

    __disable_irq();

    /* Leave no bootloader interrupt source active or pending */
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;
    for (uint32_t i = 0U; i < 8U; i++) {
        NVIC->ICER[i] = 0xFFFFFFFFU;
        NVIC->ICPR[i] = 0xFFFFFFFFU;
    }
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;

    SCB->VTOR = addr;           /* Relocate vector table       */
    __set_MSP(app_stack);       /* Set main stack pointer      */
    __DSB();
    __ISB();
    __enable_irq();             /* Reset state: PRIMASK clear  */

    app_entry_t entry = (app_entry_t)app_reset;
    entry();                    /* Jump to application Reset_Handler */
}

int main(void)
{
    if (verify_image(APP_SLOT_ADDR, APP_SLOT_SIZE) != 0) {
        /* Verification failed --- do not boot.
         * In production: attempt recovery image or signal error via LED. */
        while (1) {
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14); /* Red LED */
            for (volatile int d = 0; d < 500000; d++);
        }
    }

    jump_to_application(APP_SLOT_ADDR + IMG_HDR_SIZE);
    /* Never reached */
    return 0;
}
