/*
 * Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr
 * Chapter 12: Debugging, Tracing, and Runtime Diagnostics
 * 12.11 Lab 12: Building a Flight Recorder and Diagnostic Console > Part B: Diagnostic Console
 * Verification: executed
 * SPDX-License-Identifier: MIT
 */
/* Console command handlers */
static void cmd_tasks(void)
{
    char buf[512];
    printf("--- Task List ---\n");
    printf("Name          State Prio Stack  Num\n");
    vTaskList(buf);
    printf("%s", buf);
}

static void cmd_stats(void)
{
    char buf[512];
    printf("--- Runtime Stats ---\n");
    printf("Name          Abs Time      %% Time\n");
    vTaskGetRunTimeStats(buf);
    printf("%s", buf);
}

static void cmd_trace(void)
{
    FR_Dump();
}

static void cmd_fault(void)
{
    printf("Triggering deliberate hard fault...\n");
    HAL_Delay(100);  /* Flush UART */

    /* Word-aligned so BFAR reports exactly this address */
    volatile uint32_t *bad_ptr = (volatile uint32_t *)0xDEADDEAC;
    (void)*bad_ptr;
}

typedef struct {
    const char *name;
    void (*handler)(void);
    const char *help;
} ConsoleCommand;

static const ConsoleCommand commands[] = {
    { "tasks", cmd_tasks, "Show task list with stack usage"   },
    { "stats", cmd_stats, "Show per-task CPU utilization"     },
    { "trace", cmd_trace, "Dump flight recorder contents"     },
    { "fault", cmd_fault, "Trigger deliberate hard fault"     },
    { NULL,    NULL,      NULL                                }
};

void vConsoleTask(void *pvParameters)
{
    char line[64];

    printf("\n=== RTOS Diagnostic Console ===\n");
    printf("Commands: tasks, stats, trace, fault, help\n> ");

    for (;;) {
        if (uart_readline(line, sizeof(line), portMAX_DELAY)) {
            /* Strip trailing newline */
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n') line[len-1] = '\0';
            if (len > 1 && line[len-2] == '\r') line[len-2] = '\0';

            if (strcmp(line, "help") == 0) {
                for (int i = 0; commands[i].name; i++)
                    printf("  %-8s %s\n", commands[i].name,
                           commands[i].help);
            } else {
                int found = 0;
                for (int i = 0; commands[i].name; i++) {
                    if (strcmp(line, commands[i].name) == 0) {
                        commands[i].handler();
                        found = 1;
                        break;
                    }
                }
                if (!found)
                    printf("Unknown command: '%s'\n", line);
            }
            printf("> ");
        }
    }
}
