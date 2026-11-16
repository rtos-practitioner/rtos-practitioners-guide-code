#!/usr/bin/env bash
# Fetch the third-party sources the harness builds against into deps/ (pinned tags, no .git history).
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p deps
cd deps

fetch() {   # name url tag
  if [ -d "$1" ]; then echo "have   $1"; return; fi
  echo "clone  $1 @ $3"
  git clone -q --depth 1 --branch "$3" "$2" "$1" 2>&1 | grep -v -i "detached HEAD\|advice\|git switch\|state without\|commit them\|undo\|turn off\|^$" || true
  rm -rf "$1/.git"
}

fetch FreeRTOS-Kernel     https://github.com/FreeRTOS/FreeRTOS-Kernel.git                     V10.6.1   # the version the book pins (Appendix F)
fetch cmsis_device_f4     https://github.com/STMicroelectronics/cmsis_device_f4.git          v2.6.11
fetch cmsis_core          https://github.com/STMicroelectronics/cmsis_core.git               v5.9.0_20250520
fetch stm32f4xx_hal_driver https://github.com/STMicroelectronics/stm32f4xx_hal_driver.git    v1.8.6
fetch Unity               https://github.com/ThrowTheSwitch/Unity.git                        v2.7.0    # on-target unit tests (Ch.14)
echo "dependencies ready in $(pwd)"
