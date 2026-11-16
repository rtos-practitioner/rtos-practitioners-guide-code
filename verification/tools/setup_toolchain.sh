#!/usr/bin/env bash
# Make sure arm-none-eabi-gcc (with newlib) and qemu-system-arm are available.
#  - already installed        -> nothing to do
#  - root / passwordless sudo -> apt-get install
#  - no root                  -> download the .debs and unpack the ARM toolchain into ~/.local/arm-gnu
#                                (QEMU itself still needs a system install)
set -euo pipefail

need_qemu() { ! command -v qemu-system-arm >/dev/null 2>&1; }
need_gcc()  { ! command -v arm-none-eabi-gcc >/dev/null 2>&1 && [ ! -x "$HOME/.local/arm-gnu/usr/bin/arm-none-eabi-gcc" ]; }

if ! need_gcc && ! need_qemu; then echo "toolchain and QEMU already present"; exit 0; fi

SUDO=""
if [ "$(id -u)" -ne 0 ]; then SUDO="sudo"; fi

if command -v apt-get >/dev/null 2>&1 && { [ "$(id -u)" -eq 0 ] || sudo -n true 2>/dev/null; }; then
  $SUDO apt-get update -qq
  $SUDO apt-get install -y gcc-arm-none-eabi libnewlib-arm-none-eabi qemu-system-arm
  exit 0
fi

if need_gcc && command -v apt-get >/dev/null 2>&1; then
  echo "no root: unpacking the ARM toolchain into ~/.local/arm-gnu"
  tmp="$(mktemp -d)"; ( cd "$tmp" && apt-get download gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libnewlib-dev )
  mkdir -p "$HOME/.local/arm-gnu"
  for d in "$tmp"/*.deb; do dpkg -x "$d" "$HOME/.local/arm-gnu"; done
  rm -rf "$tmp"
  echo "installed; tools/rtos_qemu.py finds it automatically (or: export ARM_ROOT=$HOME/.local/arm-gnu/usr)"
fi
if need_qemu; then echo "qemu-system-arm is missing: ask an admin to 'apt install qemu-system-arm' (tested with 6.2)"; exit 1; fi
