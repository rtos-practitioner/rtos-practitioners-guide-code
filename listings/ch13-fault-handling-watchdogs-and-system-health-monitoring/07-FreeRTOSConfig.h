/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 13: Fault Handling, Watchdogs, and System Health Monitoring
 * 13.6 Assertion Framework > 13.6.1 configASSERT Implementation
 * Verification: executed (helper functions called with test arguments)
 * SPDX-License-Identifier: MIT
 */
/* FreeRTOSConfig.h */

#ifdef DEBUG
  #define configASSERT(x)                                            \
      do {                                                           \
          if (!(x)) {                                                \
              __disable_irq();                                       \
              assert_log(__FILE__, __LINE__, #x);                    \
              __BKPT(0);    /* Break into debugger if attached */    \
              for (;;) {}   /* Halt if no debugger              */   \
          }                                                          \
      } while (0)
#else
  #define configASSERT(x)                                            \
      do {                                                           \
          if (!(x)) {                                                \
              __disable_irq();                                       \
              fault_log_to_backup_sram(__LINE__,                     \
                  (uint32_t)__FILE__, 0xA55E0000, 0);               \
              NVIC_SystemReset();                                    \
          }                                                          \
      } while (0)
#endif

void assert_log(const char *file, int line, const char *expr)
{
    /* Minimal UART output — polling mode, no interrupts needed */
    uart_poll_print("ASSERT FAILED: ");
    uart_poll_print(file);
    uart_poll_print(":");
    uart_poll_print_int(line);
    uart_poll_print(" — ");
    uart_poll_print(expr);
    uart_poll_print("\r\n");
}
