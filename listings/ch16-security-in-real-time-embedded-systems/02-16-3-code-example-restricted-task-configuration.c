/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 16: Security in Real-Time Embedded Systems
 * 16.3 Memory Protection Unit (MPU) for Security > Code Example: Restricted Task Configuration
 * Verification: needs the FreeRTOS-MPU port (not built by the harness)
 * SPDX-License-Identifier: MIT
 */
#include "FreeRTOS.h"
#include "task.h"

/* Shared read-only configuration (aligned for MPU region) */
static const __attribute__((aligned(256))) uint8_t
    g_config[256] = { /* ... populated at init ... */ };

/* Task-private data buffer (aligned for MPU region) */
static __attribute__((aligned(1024))) uint8_t
    g_network_buf[1024];

/* Task function running in unprivileged mode */
static void vNetworkParserTask(void *pvParam)
{
    /* This task can read g_config and read/write g_network_buf.
     * Any access to other memory regions triggers MemManage fault. */
    for (;;) {
        uint16_t len = receive_packet(g_network_buf, sizeof(g_network_buf));
        if (len > 0) {
            parse_packet(g_network_buf, len, g_config);
        }
    }
}

void vCreateRestrictedNetworkTask(void)
{
    static StackType_t xStack[512] __attribute__((aligned(2048)));

    /* Define the three user-configurable MPU regions */
    static const TaskParameters_t xTaskParams = {
        .pvTaskCode   = vNetworkParserTask,
        .pcName       = "NetParse",
        .usStackDepth = 512,
        .pvParameters = NULL,
        .uxPriority   = (tskIDLE_PRIORITY + 2) | portPRIVILEGE_BIT_CLEAR,
        .puxStackBuffer = xStack,
        .xRegions = {
            /* Region 0: task data buffer, read-write, no execute */
            { g_network_buf, 1024,
              portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER },
            /* Region 1: shared config, read-only, no execute */
            { (void *)g_config, 256,
              portMPU_REGION_READ_ONLY | portMPU_REGION_EXECUTE_NEVER },
            /* Region 2: unused */
            { 0, 0, 0 }
        }
    };

    xTaskCreateRestricted(&xTaskParams, NULL);
}
