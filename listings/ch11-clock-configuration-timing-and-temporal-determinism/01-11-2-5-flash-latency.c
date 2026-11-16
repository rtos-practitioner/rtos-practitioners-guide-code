/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 11: Clock Configuration, Timing, and Temporal Determinism
 * 11.2 STM32F407 Clock Architecture > 11.2.5 Flash Latency
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Configure flash latency before increasing clock speed */
FLASH->ACR = FLASH_ACR_LATENCY_5WS   /* 5 wait states */
           | FLASH_ACR_ICEN           /* instruction cache enable */
           | FLASH_ACR_DCEN           /* data cache enable */
           | FLASH_ACR_PRFTEN;        /* prefetch enable */
