# Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
# Appendix F: Development Environment Setup Guide
# F.6 FreeRTOS Source Integration > F.6.2 Required Source Files
# Verification: not run (host command / other RTOS / configuration)
# SPDX-License-Identifier: MIT
set(FREERTOS_SRC
    third-party/FreeRTOS/tasks.c
    third-party/FreeRTOS/queue.c
    third-party/FreeRTOS/list.c
    third-party/FreeRTOS/timers.c
    third-party/FreeRTOS/event_groups.c
    third-party/FreeRTOS/stream_buffer.c
    third-party/FreeRTOS/portable/GCC/ARM_CM4F/port.c
    third-party/FreeRTOS/portable/MemMang/heap_4.c
)

target_include_directories(app PRIVATE
    include                                          # FreeRTOSConfig.h
    third-party/FreeRTOS/include                     # FreeRTOS headers
    third-party/FreeRTOS/portable/GCC/ARM_CM4F       # portmacro.h
)
