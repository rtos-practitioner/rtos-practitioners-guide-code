/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 7: Memory Management in Resource-Constrained Systems
 * 7.5 Memory Protection Unit (MPU) > FreeRTOS-MPU Port
 * Verification: needs the FreeRTOS-MPU port (not built by the harness)
 * SPDX-License-Identifier: MIT
 */
static StackType_t xTaskStack[512] __attribute__((aligned(2048)));
static uint8_t     xTaskData[256]  __attribute__((aligned(256)));

static const TaskParameters_t xTaskDef = {
    .pvTaskCode     = vUntrustedTask,
    .pcName         = "Untrusted",
    .usStackDepth   = 512,
    .pvParameters   = NULL,
    .uxPriority     = tskIDLE_PRIORITY | portPRIVILEGE_BIT, /* remove bit for unpriv */
    .puxStackBuffer = xTaskStack,
    .xRegions       = {
        /* Base address,    Length (bytes),  Permissions                      */
        { xTaskData,        256,            portMPU_REGION_READ_WRITE        },
        { (void *)0x40020000, 0x400,        portMPU_REGION_READ_WRITE        }, /* GPIOA */
        { 0, 0, 0 }  /* Unused region */
    }
};

xTaskCreateRestricted(&xTaskDef, &xHandle);
