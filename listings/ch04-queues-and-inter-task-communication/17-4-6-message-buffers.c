/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 4: Queues and Inter-Task Communication
 * 4.6 Message Buffers and Stream Buffers > Message Buffers
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Send a variable-length log entry */
size_t xSent = xMessageBufferSend( xMsgBuf,
                                    pucLogEntry, uxEntryLen,
                                    pdMS_TO_TICKS( 20 ) );

/* Receive the next complete message */
size_t xReceived = xMessageBufferReceive( xMsgBuf,
                                           ucRxBuf, sizeof( ucRxBuf ),
                                           portMAX_DELAY );
