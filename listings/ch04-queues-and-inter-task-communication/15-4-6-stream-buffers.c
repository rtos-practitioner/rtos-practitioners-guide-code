/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.6 Message Buffers and Stream Buffers > Stream Buffers
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Writer (e.g., UART DMA complete callback) */
size_t xBytesSent = xStreamBufferSend( xStreamBuf,
                                        pucData, uxLen,
                                        pdMS_TO_TICKS( 10 ) );

/* Reader */
size_t xBytesRead = xStreamBufferReceive( xStreamBuf,
                                           ucLocalBuf, sizeof( ucLocalBuf ),
                                           portMAX_DELAY );
