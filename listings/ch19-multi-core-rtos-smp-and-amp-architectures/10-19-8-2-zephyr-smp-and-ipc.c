/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 19: Multi-Core RTOS: SMP and AMP Architectures
 * 19.8 QNX and Zephyr Multi-Core > 19.8.2 Zephyr SMP and IPC
 * Verification: not run (host command / other RTOS / configuration)
 * SPDX-License-Identifier: MIT
 */
/* Zephyr IPC service — send a message to the remote core */
const struct device *ipc = DEVICE_DT_GET(DT_NODELABEL(ipc0));
struct ipc_ept ep;
ipc_service_open_instance(ipc);
ipc_service_register_endpoint(ipc, &ep, &ep_cfg);
ipc_service_send(&ep, data, sizeof(data));
