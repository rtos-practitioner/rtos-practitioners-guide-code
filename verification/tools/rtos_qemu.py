"""rtos_qemu.py - build FreeRTOS + STM32 HAL + board layer once, then build and run
single "units" (wrapped book listings) on QEMU netduinoplus2 (STM32F405 Cortex-M4F).
"""
import os
import re
import subprocess
import time
import glob

PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def _find_arm_root():
    """Prefix that contains bin/arm-none-eabi-gcc and lib/arm-none-eabi/newlib (e.g. /usr or ~/.local/arm-gnu/usr)."""
    if os.environ.get("ARM_ROOT"):
        return os.environ["ARM_ROOT"]
    import shutil
    gcc = shutil.which("arm-none-eabi-gcc")
    if gcc:
        return os.path.dirname(os.path.dirname(os.path.realpath(gcc)))
    return os.path.expanduser("~/.local/arm-gnu/usr")


ARM_ROOT = _find_arm_root()
CC = os.path.join(ARM_ROOT, "bin", "arm-none-eabi-gcc")
AR = os.path.join(ARM_ROOT, "bin", "arm-none-eabi-ar")
SIZE = os.path.join(ARM_ROOT, "bin", "arm-none-eabi-size")
NEWLIB = os.path.join(ARM_ROOT, "lib", "arm-none-eabi", "newlib")
QEMU = os.environ.get("QEMU", "qemu-system-arm")

DEPS = os.path.join(PROJ, "deps")
BUILD = os.path.join(PROJ, "build")
PLAT = os.path.join(PROJ, "platform")
KERNEL = os.path.join(DEPS, "FreeRTOS-Kernel")
HALDIR = os.path.join(DEPS, "stm32f4xx_hal_driver")

ARCH = ["-mcpu=cortex-m4", "-mthumb", "-mfpu=fpv4-sp-d16", "-mfloat-abi=hard"]
INCLUDES = [
    PLAT, os.path.join(PLAT, "generated"), os.path.join(PLAT, "shim"),
    os.path.join(DEPS, "cmsis_device_f4", "Include"),
    next(d for d in (os.path.join(DEPS, "cmsis_core", "Include"),                      # tagged releases
                     os.path.join(DEPS, "cmsis_core", "CMSIS", "Core", "Include"))      # newer master layout
         if os.path.exists(os.path.join(d, "core_cm4.h"))) if os.path.isdir(os.path.join(DEPS, "cmsis_core")) else "",
    os.path.join(HALDIR, "Inc"),
    os.path.join(KERNEL, "include"),
    os.path.join(KERNEL, "portable", "GCC", "ARM_CM4F"),
]
SYSINC = [os.path.join(ARM_ROOT, "include", "newlib", "nano"), os.path.join(ARM_ROOT, "include", "newlib")]
DEFS = ["-DSTM32F407xx", "-DUSE_HAL_DRIVER"]
CFLAGS = (ARCH + ["-O1", "-g", "-std=gnu11", "-ffunction-sections", "-fdata-sections", "-fno-common"]
          + DEFS + [f"-I{p}" for p in INCLUDES] + [f"-isystem{p}" for p in SYSINC])
UNIT_WARN = ["-w"]  # book listings are judged on errors; warnings are collected separately
LDFLAGS = (ARCH + ["-T", os.path.join(PLAT, "STM32F405_QEMU.ld"), "-nostartfiles", "-nodefaultlibs",
                   "-Wl,--gc-sections", "-Wl,-Map=%s", f"-L{NEWLIB}/thumb/v7e-m+fp/hard",
                   "-u", "_printf_float", "-u", "_scanf_float"])
LIBS = ["-Wl,--start-group", "-lc_nano", "-lm", "-lgcc", "-Wl,--end-group"]

KERNEL_SRCS = ["tasks.c", "queue.c", "list.c", "timers.c", "event_groups.c", "stream_buffer.c"]
HAL_SRCS = ["hal", "hal_cortex", "hal_gpio", "hal_exti", "hal_dma", "hal_dma_ex", "hal_rcc", "hal_rcc_ex",
            "hal_pwr", "hal_pwr_ex", "hal_flash", "hal_flash_ex", "hal_adc", "hal_adc_ex", "hal_can",
            "hal_crc", "hal_dac", "hal_dac_ex", "hal_i2c", "hal_i2c_ex", "hal_iwdg", "hal_rng",
            "hal_rtc", "hal_rtc_ex", "hal_spi", "hal_tim", "hal_tim_ex", "hal_uart", "hal_usart", "hal_wwdg"]


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def _compile(src, obj, extra=()):
    os.makedirs(os.path.dirname(obj), exist_ok=True)
    r = run([CC, *CFLAGS, *extra, "-c", src, "-o", obj])
    if r.returncode != 0:
        raise RuntimeError(f"compile failed: {src}\n{r.stderr[-3000:]}")


def build_platform(force=False, jobs=16):
    """Compile kernel, HAL archive, board, harness, startup, system. Idempotent."""
    from concurrent.futures import ThreadPoolExecutor
    import shutil
    out = os.path.join(BUILD, "platform")
    if force and os.path.isdir(out):
        shutil.rmtree(out)                      # no stale objects (e.g. an old heap_4.o)
    os.makedirs(out, exist_ok=True)
    stamp = os.path.join(out, ".stamp")
    deps_mtime = max(os.path.getmtime(p) for p in glob.glob(os.path.join(PLAT, "*")) +
                     glob.glob(os.path.join(PLAT, "generated", "*")) + glob.glob(os.path.join(PLAT, "shim", "*")))
    if not force and os.path.exists(stamp) and os.path.getmtime(stamp) > deps_mtime:
        return out
    jobsl = []
    objs = {}
    for s in KERNEL_SRCS:
        objs[s] = os.path.join(out, "kernel", s[:-2] + ".o")
        jobsl.append((os.path.join(KERNEL, s), objs[s], ()))
    port = os.path.join(KERNEL, "portable", "GCC", "ARM_CM4F", "port.c")
    jobsl.append((port, os.path.join(out, "kernel", "port.o"), ()))
    for heap in ("heap_4", "heap_5"):          # heap_4 default; units that call vPortDefineHeapRegions use heap_5
        jobsl.append((os.path.join(KERNEL, "portable", "MemMang", heap + ".c"),
                      os.path.join(out, "heap", heap + ".o"), ()))
    for name in ("board", "harness"):
        jobsl.append((os.path.join(PLAT, name + ".c"), os.path.join(out, name + ".o"), ()))
    jobsl.append((os.path.join(DEPS, "cmsis_device_f4", "Source", "Templates", "system_stm32f4xx.c"),
                  os.path.join(out, "system.o"), ()))
    if os.path.exists(os.path.join(DEPS, "Unity", "src", "unity.c")):      # on-target unit tests (Ch.14)
        jobsl.append((os.path.join(DEPS, "Unity", "src", "unity.c"), os.path.join(out, "unity", "unity.o"),
                      (f"-I{os.path.join(DEPS, 'Unity', 'src')}",)))
    hal_objs = []
    for h in HAL_SRCS:
        src = os.path.join(HALDIR, "Src", f"stm32f4xx_{h}.c")
        obj = os.path.join(out, "hal", f"{h}.o")
        hal_objs.append(obj)
        jobsl.append((src, obj, ()))
    with ThreadPoolExecutor(jobs) as ex:
        for f in [ex.submit(_compile, *j) for j in jobsl]:
            f.result()
    r = run([CC, *ARCH, *DEFS, "-x", "assembler-with-cpp", "-c",
             os.path.join(DEPS, "cmsis_device_f4", "Source", "Templates", "gcc", "startup_stm32f407xx.s"),
             "-o", os.path.join(out, "startup.o")])
    if r.returncode:
        raise RuntimeError(r.stderr)
    lib = os.path.join(out, "libhal.a")
    if os.path.exists(lib):
        os.remove(lib)
    r = run([AR, "rcs", lib, *hal_objs])
    if r.returncode:
        raise RuntimeError(r.stderr)
    open(stamp, "w").write(str(time.time()))
    return out


def kernel_objects(heap="heap_4"):
    out = os.path.join(BUILD, "platform")
    ks = sorted(glob.glob(os.path.join(out, "kernel", "*.o"))) + [os.path.join(out, "heap", heap + ".o")]
    ks += glob.glob(os.path.join(out, "unity", "*.o"))
    # board.o must precede startup.o so board's weak fault handlers win over the template's weak aliases
    return ks + [os.path.join(out, "harness.o"), os.path.join(out, "board.o"),
                 os.path.join(out, "system.o"), os.path.join(out, "startup.o"), os.path.join(out, "libhal.a")]


def build_unit(c_src, name, run_ms=1000, extra_cflags=(), workdir=None, extra_objs=(), deadline_tim=3):
    """Compile one generated unit and link firmware. Returns dict(ok, stage, log, elf, warnings)."""
    workdir = workdir or os.path.join(BUILD, "units", name)
    os.makedirs(workdir, exist_ok=True)
    obj = os.path.join(workdir, "unit.o")
    elf = os.path.join(workdir, "unit.elf")
    r = run([CC, *CFLAGS, "-Wall", "-Wno-unused", "-Wno-comment", f'-DUNIT_NAME="{name}"',
             f"-DUNIT_RUN_MS={run_ms}", *extra_cflags, "-c", c_src, "-o", obj])
    warns = r.stderr
    if r.returncode != 0:
        return dict(ok=False, stage="compile", log=r.stderr, elf=None, warnings="")
    # harness.c needs UNIT_NAME / UNIT_RUN_MS per unit: rebuild it cheaply
    hobj = os.path.join(workdir, "harness.o")
    r = run([CC, *CFLAGS, f'-DUNIT_NAME="{name}"', f"-DUNIT_RUN_MS={run_ms}", f"-DHARNESS_DEADLINE_TIM={deadline_tim}",
             "-c", os.path.join(PLAT, "harness.c"), "-o", hobj])
    if r.returncode != 0:
        return dict(ok=False, stage="compile-harness", log=r.stderr, elf=None, warnings="")
    heap = "heap_5" if "vPortDefineHeapRegions" in open(c_src).read() else "heap_4"
    ko = [o for o in kernel_objects(heap) if not o.endswith("harness.o")]
    ld = [a.replace("%s", os.path.join(workdir, "unit.map")) for a in LDFLAGS]
    r = run([CC, *ld, obj, *extra_objs, hobj, *ko, *LIBS, "-o", elf])
    if r.returncode != 0:
        return dict(ok=False, stage="link", log=r.stderr, elf=None, warnings=warns)
    return dict(ok=True, stage="built", log="", elf=elf, warnings=warns)


EXIT_NAMES = {0: "RAN_OK", 2: "FAULT", 3: "STACK_OVERFLOW", 4: "MALLOC_FAILED", 5: "ASSERT",
              6: "SCHED_START_FAILED", 7: "UNIT_DONE"}


def run_elf(elf, wall_timeout=60, shift=3):
    """Run firmware under QEMU. -icount makes virtual time deterministic and lets WFI skip idle time."""
    cmd = [QEMU, "-machine", "netduinoplus2", "-display", "none", "-monitor", "none", "-serial", "stdio",
           "-semihosting-config", "enable=on,target=native", "-no-reboot",
           "-icount", f"shift={shift},sleep=off",
           "-kernel", elf]
    t0 = time.time()
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=wall_timeout, errors="replace")
        out, rc, to = r.stdout, r.returncode, False
    except subprocess.TimeoutExpired as e:
        out = (e.stdout.decode("utf-8", "replace") if isinstance(e.stdout, bytes) else (e.stdout or ""))
        rc, to = -1, True
    wall = time.time() - t0
    if to:
        status = "HANG"
    elif rc in EXIT_NAMES:
        status = EXIT_NAMES[rc]
    else:
        status = "NO_EXIT_RESET"       # guest reset / exited without semihosting call
    return dict(status=status, rc=rc, output=out, wall=wall)
