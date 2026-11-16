# Chapter 16: Security in Real-Time Embedded Systems

Listings in the order they appear in the chapter. Most are excerpts that rely on declarations from earlier listings or from the lab project; see the repository README.

| File | Section | Verification |
|---|---|---|
| [01-16-2-code-example-simplified-boot-verification.c](01-16-2-code-example-simplified-boot-verification.c) | 16.2 Secure Boot and Chain of Trust > Code Example: Simplified Boot Verification | bootloader-style program (no scheduler by design) |
| [02-16-3-code-example-restricted-task-configuration.c](02-16-3-code-example-restricted-task-configuration.c) | 16.3 Memory Protection Unit (MPU) for Security > Code Example: Restricted Task Configuration | needs the FreeRTOS-MPU port (not built by the harness) |
| [03-16-4-code-example-aes-128-gcm-encryption-of-sensor-data.c](03-16-4-code-example-aes-128-gcm-encryption-of-sensor-data.c) | 16.4 Cryptographic Operations > Code Example: AES-128-GCM Encryption of Sensor Data | executed (helper functions called with test arguments) |
| [04-16-5-code-example-hmac-authenticated-can-message.c](04-16-5-code-example-hmac-authenticated-can-message.c) | 16.5 Secure Communication > Code Example: HMAC-Authenticated CAN Message | changed since the reference run - re-run tools/run_suite.py |
| [05-16-6-code-example-ota-update-state-machine.c](05-16-6-code-example-ota-update-state-machine.c) | 16.6 Over-the-Air (OTA) Firmware Updates > Code Example: OTA Update State Machine | executed |
| [06-16-10-part-b-hmac-authenticated-uart-command-protocol.c](06-16-10-part-b-hmac-authenticated-uart-command-protocol.c) | 16.10 Lab 16: Secure Boot and Authenticated Commands > Part B: HMAC-Authenticated UART Command Protocol | executed |
