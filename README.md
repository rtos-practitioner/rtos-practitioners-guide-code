# Real-Time Operating Systems — Companion Code

Code listings from **_Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr_**
by Azad Mohammed Shaik, together with the harness that compiles and runs them under FreeRTOS on an emulated
STM32F4 (Cortex-M4F).

```bash
git clone https://github.com/rtos-practitioner/rtos-practitioners-guide-code.git
```

## Repository layout

```
listings/                 one file per code listing, grouped by chapter (README.md in each folder)
  ch01-.../ ... ch19-.../
  appendix-d-.../ appendix-f-.../
verification/             build-and-run harness (QEMU netduinoplus2, FreeRTOS V10.6.1, STM32F4 HAL)
  tools/                  extract, wrap, build, run, report, export_listings.py
  platform/               board support, linker script, FreeRTOSConfig.h used for the runs
  extracted/              every code block of the book in document order (blocks.json, index.csv)
  results/                reference-run-2026-11-10/ (committed); fresh runs write here
.github/workflows/        CI: runs the whole suite on Ubuntu 22.04 on every push
```

## Using the listings

Each file in `listings/` starts with a header naming the chapter and section it comes from and how it was
verified. File names keep the book's order (`NN-`) and use the file name the book gives the listing
(`uart_rtos.c`, `ipc_shared.h`, ...) or the section number and title.

Most listings are excerpts: they rely on declarations from earlier listings in the same chapter, on the lab
project from Chapter 2, or on the STM32 HAL. The labs (`*-lab-*`, `lab*_*.c`, and the `main.c` files) are the
most complete programs. Hardware: STM32F407 Discovery or Nucleo-F446RE for Chapters 1–18, a dual-core
STM32H755 Nucleo for Lab 19, and an RP2040 board for the Chapter 19 SMP examples. QNX and Zephyr listings in the
portability sections need those SDKs.

## Verification

`verification/` compiles every C listing with `arm-none-eabi-gcc` against FreeRTOS V10.6.1 (ARM_CM4F port),
CMSIS and the STM32F4 HAL, and runs it on QEMU's `netduinoplus2` machine. Reference run (2026-11-10):

| Result | C listings |
|---|---:|
| Executed on the target | 151 |
| Executed via helper-function calls | 45 |
| Compiled, linked and booted (declarations only) | 90 |
| Needs hardware QEMU does not model / the MPU port | 6 |
| Anti-pattern that fails exactly as the book describes | 2 |
| By design (bare-metal loop, bootloader, excerpt) | 4 |
| Book issues — **fixed since the reference run** | 3 |

To reproduce (Ubuntu 22.04, QEMU 6.2, arm-none-eabi-gcc 10.3):

```bash
cd verification
bash tools/setup_toolchain.sh
pip install -r requirements.txt
bash tools/fetch_deps.sh
python3 tools/run_suite.py --jobs "$(nproc)"
python3 tools/report.py            # results/RESULTS.md
python3 tools/export_listings.py   # refreshes the status in listings/
```

Details, deviations from real hardware, and single-listing runs: [verification/README.md](verification/README.md).
Emulator timing is not cycle-accurate; reproduce latency and jitter measurements on real hardware.

## License

The code is released under the [MIT License](LICENSE). The book text is not part of this repository and remains
© 2026 Azad Mohammed Shaik, all rights reserved. Third-party sources (FreeRTOS, CMSIS, STM32 HAL, Unity) are
downloaded by `verification/tools/fetch_deps.sh` under their own licenses and are not redistributed here.

The examples are for education. Verify them on your own hardware before using them in a product, and do not
use them in safety-critical systems without the analysis and certification the application requires.
