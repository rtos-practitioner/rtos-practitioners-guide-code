# Verification harness: book listings on FreeRTOS / STM32F4 (QEMU)

Runs every code block of the book on **FreeRTOS V10.6.1** (the version the book's
Appendix F pins) using the real **ARM_CM4F port**, the real **STM32F4 HAL / CMSIS**, on **QEMU `netduinoplus2`**
(STM32F405, Cortex-M4F). All commands below run from this `verification/` folder. A fresh run writes
`results/RESULTS.md` (human) and `results/results.json` (machine); the committed reference run is in
`results/reference-run-2026-11-10/`.

## Quick start (compile and run)

Tested on Ubuntu 22.04 with QEMU 6.2, `arm-none-eabi-gcc` 10.3, Python 3.10. Newer QEMU versions model the NVIC,
DWT and SysTick differently and the platform shims in `platform/` assume 6.2 behaviour.

```bash
# 1. prerequisites: ARM GCC + newlib, QEMU, python-docx
bash tools/setup_toolchain.sh        # apt install if you have sudo, else unpacks the ARM toolchain into ~/.local
pip install -r requirements.txt
#    (without root, QEMU must be installed by an admin: apt install qemu-system-arm)

# 2. fetch FreeRTOS V10.6.1, STM32F4 HAL, CMSIS, Unity into deps/ (pinned tags)
bash tools/fetch_deps.sh

# 3. (only if you own the book .docx and want to re-extract) docx -> extracted/
python3 tools/extract_blocks.py /path/to/RTOS_Book_KDP_Paperback_7x10.docx

# 4. build FreeRTOS + HAL once, then wrap, compile and run every C listing on QEMU (~75 s on 20 jobs)
python3 tools/run_suite.py --jobs 20

# 5. write the report
python3 tools/report.py              # -> results/RESULTS.md, results/summary.json, extracted/ALL_CODE_BLOCKS.md

# 6. refresh ../listings/ (one file per listing, with the verification status in each header)
python3 tools/export_listings.py
```

The first `run_suite.py` call builds the kernel/HAL/board layer into `build/platform/` (a few seconds); later calls
reuse it (`--rebuild` forces a clean rebuild). Summary line of the reference run (before the book fixes below):
`RAN_OK 286, NOT_EXECUTABLE 135, BUILD_FAIL 7, SCHED_NOT_STARTED 3, FAULT 3, ASSERT 2`.

### Run a single listing

```bash
python3 tools/run_suite.py --only 11          # block 11 = Lab 1 timing; also 51 (Lab 3), 28,29,30 (Ch.2 tasks)
cat build/units/b011/run.log                  # what it printed on the target
python3 tools/dbg_block.py 252 -v             # show how block 252 is wrapped (stubs, glue)
```

Per unit, `build/units/bNNN/` holds `unit.c` (the generated source, book listing included verbatim),
`unit.elf`, `unit.map` and `run.log`. Assembled labs are `build/units/labSxxx/`; synthetic helper runs end in `h`.
To start a firmware by hand:

```bash
qemu-system-arm -machine netduinoplus2 -display none -monitor none -serial stdio \
  -semihosting-config enable=on,target=native -no-reboot -icount shift=3,sleep=off \
  -kernel build/units/b011/unit.elf
```

QEMU's exit code is the verdict: `0` ran its whole window cleanly, `2` fault, `3` stack overflow, `4` malloc failed,
`5` `configASSERT`, `6` scheduler failed to start. The last lines of every log are `[HARNESS] ... RESULT RAN_OK`.

### Run your own FreeRTOS code on the same target

```c
/* unit.c */
#include "unit_prelude.h"                      /* FreeRTOS, STM32 HAL, stdio, ... */
static void hello(void *p)
{
    for (int i = 0; i < 3; i++) { printf("hello tick=%lu\r\n", (unsigned long)xTaskGetTickCount()); vTaskDelay(pdMS_TO_TICKS(100)); }
    vTaskDelete(NULL);
}
void unit_setup(void) { xTaskCreate(hello, "hello", 512, NULL, 2, NULL); }   /* harness starts the scheduler */
```

```bash
python3 -c "import sys; sys.path.insert(0,'tools'); import rtos_qemu as q; r=q.build_unit('unit.c','mine',run_ms=2000); print(q.run_elf(r['elf'])['output'])"
```

prints `hello tick=0`, `100`, `200` and `RESULT RAN_OK` after the 2000 ms virtual-time window.

### Troubleshooting

| symptom | fix |
|---|---|
| `arm-none-eabi-gcc: not found` | run `tools/setup_toolchain.sh`, or `export ARM_ROOT=<prefix containing bin/arm-none-eabi-gcc>` |
| `stdio.h: No such file` | newlib missing: `apt install libnewlib-arm-none-eabi` (no-root path: `setup_toolchain.sh`) |
| `core_cm4.h: No such file` | `deps/` incomplete; delete `deps/cmsis_core` and re-run `tools/fetch_deps.sh` |
| everything `HANG` / FreeRTOS asserts at start | QEMU other than 6.2: see the note above |

## How a listing becomes a test

`tools/wrap.py` parses each block and picks a shape, confirming by compiling:

| shape | what runs |
|---|---|
| program (`main`) | `main` renamed `book_main`, called by the harness; the listing starts the scheduler itself |
| library (functions) | task bodies are spawned, init functions / ISRs (`*_IRQHandler`, pended through the NVIC) are run |
| fragment (bare statements) | wrapped in a function and run in a task |
| mixed (functions + usage snippet) | definitions at file scope, snippet run in a task |
| declarations | compiled, linked, booted |

Missing context is supplied automatically and **reported per block** (`stubs` in `results.json`): kernel objects
behind handle variables, HAL handles, constants, structs/types, undefined helper functions (weak link stubs),
other listings of the book (`/* src/foo.h */`-style project files and the preceding listings of the section/chapter),
Unity for the Chapter 14 tests. Whole labs/sections are additionally assembled and run as one program.
Helper functions with arguments are exercised in a **separate** run with synthesized arguments; failures there are
not counted against the book.

Each run has a virtual-time window (1 s, longer if the code uses longer delays) ended by a timer interrupt, so
infinite task loops terminate deterministically. Exit status comes back through QEMU semihosting.

## Deviations from a real STM32F407 (all in `platform/`, book code is never edited)

* **FreeRTOSConfig.h** = the book's Appendix D.9 verbatim + required additions (queue sets, idle/tick hooks,
  run-time-stats timer, INCLUDE_* switches). `configASSERT` prints and exits QEMU instead of spinning.
* **DWT** does not exist in QEMU 6.2: `DWT->CYCCNT/CTRL` (and raw `0xE0001000/4` accesses, rewritten by the
  generator) are mapped onto TIM4, 166.67 MHz equivalent (0.8 % below 168 MHz).
* **Time**: `-icount shift=3,sleep=off` (8 ns/instruction ≈ 1.3 cycles/instruction at 168 MHz). `WFI/WFE` are NOPs
  (QEMU 6.2 mis-times SysTick in WFI with `sleep=off`); SysTick is then exact and every log is reproducible
  (374 of 375 logs byte-identical across two full runs).
* **NVIC**: QEMU implements 8 priority bits (STM32F4: 4); FreeRTOS then needs PRIGROUP 0, so
  `NVIC_SetPriorityGrouping()` is forced to 0 (priority encodings are unchanged).
* **No CCM RAM / backup SRAM / RCC / PWR / IWDG / RTC / ITM** behaviour in QEMU: `.ccmram` is placed in SRAM;
  listings that poll those peripherals' ready flags or hard-code those addresses are reported as `HW_LIMITED`.
* Tickless idle, the MPU port and STM32H7 dual-core are not built (reported per block).

## Findings from the reference run (block numbers refer to that run)

Fixed in the book since the reference run; `extracted/` already holds the corrected listings:

* Ch.19.4 used `telemetry_ring` from Lab 19's `ipc_shared.h`, which defines `sensor_ring` (blocks 399, 400):
  Section 19.4 now has its own `amp_ipc.h`.
* Ch.16 called `can_receive()` with 4 arguments; `can_driver.h` declares 2 (block 365): now uses `can_msg_t`.
* Excerpts with elided arguments / a bare `if (...)` (358, 393) are now complete statements.
* Backup-regulator polling loops (blocks 280, 303, 305) are now bounded, so `main()` reaches the scheduler.
* Warnings in blocks 15, 202, 280, 333, 408, 409 (discarded `volatile`, missing return, missing declarations).

Expected, not defects:

* Ch.15 anti-pattern listings fail in the way the book says they should (342, 357).
* CCM RAM, ITM, the FreeRTOS-MPU port and STM32H7 memory are not available in QEMU (`HW_LIMITED`, `NEEDS_OTHER_PORT`).
