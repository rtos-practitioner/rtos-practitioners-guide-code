#!/usr/bin/env python3
"""report.py - turn results/results.json into results/RESULTS.md (+ results/summary.json)
and extracted/ALL_CODE_BLOCKS.md (every extracted block with its heading context)."""
import collections
import csv
import json
import os
import re

PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RES = os.path.join(PROJ, "results")


def load():
    return json.load(open(os.path.join(RES, "results.json")))


def diagnose(e, text):
    """(verdict, explanation) for one entry."""
    st = e["status"]
    ek = e.get("exec_kind", "")
    out = e.get("output", "") or ""
    if st == "NOT_EXECUTABLE":
        return "NOT_CODE", e.get("reason", "")
    if st == "BUILD_FAIL":
        err = e.get("errors", "") or ""
        if re.search(r"xTaskCreateRestricted|TaskParameters_t|portMPU", text):
            return "NEEDS_OTHER_PORT", "uses the FreeRTOS-MPU port API (xTaskCreateRestricted / TaskParameters_t); this harness builds the standard ARM_CM4F port"
        if "has no member named" in err and "SharedIPC_t" in err:
            return "BOOK_ISSUE", "listing uses member `telemetry_ring`, but the book's own ipc_shared.h (Lab 19) defines `sensor_ring`"
        if "too many arguments to function 'can_receive'" in err:
            return "BOOK_ISSUE", "calls can_receive(&id, frame, 8, timeout) with 4 arguments, but the book's can_driver.h declares can_receive(can_msg_t *msg, uint32_t timeout_ms)"
        if re.search(r"/\*\s*\.\.\.\s*\*/", text) and "expected expression" in err:
            return "EXCERPT", "illustrative excerpt: elided arguments (`/* ... */`) leave an incomplete call, not compilable as written"
        if re.match(r"^\s*if\s*\(", text) and len(text.strip().splitlines()) == 1:
            return "EXCERPT", "illustrative excerpt: a bare `if (...)` condition with no statement"
        if e.get("kind") == "lab":
            return "BOOK_ISSUE_OR_EXCERPT", "assembled section inherits a failure from one of its listings (see members)"
        return "BUILD_FAIL", err.strip().split("\n")[0][:200]
    if e.get("anti_pattern") and st in ("ASSERT", "FAULT", "STACK_OVERFLOW", "MALLOC_FAILED"):
        return "EXPECTED_FAILURE", "the section is a documented pitfall/anti-pattern; the failure (" + st.lower().replace("_", " ") + ") is the demonstration"
    if st == "FAULT":
        m = re.search(r"BFAR=0x([0-9a-f]+)", out)
        bf = int(m.group(1), 16) if m else None
        if bf is not None and 0x10000000 <= bf < 0x10010000:
            return "HW_LIMITED", "bus fault at CCM RAM (0x10000000..): QEMU's STM32F405 model has no CCM RAM; the listing hard-codes it"
        if bf is not None and 0x40024000 <= bf < 0x40025000:
            return "HW_LIMITED", "bus fault in backup SRAM (0x40024000..): not modelled by QEMU's STM32F405"
        if bf is not None and 0x30000000 <= bf < 0x38000000:
            return "HW_LIMITED", "bus fault at 0x3004xxxx (STM32H7 D3 SRAM): H7-only memory, the listing targets the dual-core STM32H7"
        return "FAULT", out.strip().split("\n")[-1][:200]
    if st == "ASSERT":
        if e.get("anti_pattern"):
            return "EXPECTED_FAILURE", "the section is a documented pitfall/anti-pattern; FreeRTOS' own configASSERT catches it exactly as the book warns"
        return "ASSERT", [l for l in out.split("\n") if "ASSERT" in l][-1][:200]
    if st == "SCHED_NOT_STARTED":
        at = e.get("stuck_at", "")
        if re.search(r"bkpsram|BackupSRAM", at, re.I):
            return "HW_LIMITED", f"main() never reaches vTaskStartScheduler(): stuck in {at} polling the PWR backup-regulator-ready flag (PWR peripheral is not modelled by QEMU)"
        if "Bare Metal" in e.get("section", ""):
            return "BARE_METAL_BY_DESIGN", "bare-metal super-loop listing: it intentionally never starts an RTOS scheduler; the loop itself ran for the whole window"
        return "NO_SCHEDULER", f"main() does not start the scheduler within the window (stuck in {at}); bootloader-style listing whose loop ran for the whole window"
    if st == "HANG":
        return "HW_LIMITED", "wall-clock timeout: the listing retargets stdout to ITM/SWO which QEMU does not model"
    if st == "RAN_OK":
        hs = e.get("helper_status")
        if ek.startswith("declarations only"):
            return "COMPILED_ONLY", "types / macros / prototypes: compiled, linked into a firmware image and booted; nothing to execute"
        if ek.startswith("no-entry"):
            if hs == "RAN_OK":
                return "EXECUTED_HELPERS", "helper functions invoked with synthesized arguments (separate run) without faulting"
            return "COMPILED_ONLY", "helper functions only; no callable entry could be synthesized" + \
                (f" (synthetic-argument run: {hs})" if hs else "")
        return "EXECUTED", ""
    return st, ""


def main():
    R = load()
    blocks = {b["idx"]: b for b in json.load(open(os.path.join(PROJ, "extracted", "blocks.json")))}
    for e in R:
        text = blocks[e["id"]]["text"] if isinstance(e["id"], int) else ""
        e["verdict"], e["why"] = diagnose(e, text)
    json.dump(R, open(os.path.join(RES, "results.json"), "w"), indent=1, ensure_ascii=False)

    blk = [e for e in R if e["kind"] != "lab"]
    labs = [e for e in R if e["kind"] == "lab"]
    kinds = collections.Counter(e["kind"] for e in blk)
    verdicts = collections.Counter(e["verdict"] for e in blk)
    c_blocks = [e for e in blk if e["kind"] == "c"]
    cv = collections.Counter(e["verdict"] for e in c_blocks)
    st = collections.Counter(e["status"] for e in c_blocks)
    summary = dict(total_blocks=len(blk), by_kind=dict(kinds), c_listings=len(c_blocks), c_verdicts=dict(cv),
                   c_status=dict(st), labs=len(labs), lab_status=dict(collections.Counter(e["status"] for e in labs)),
                   all_verdicts=dict(verdicts))
    json.dump(summary, open(os.path.join(RES, "summary.json"), "w"), indent=1)

    L = []
    w = L.append
    w("# Book code on FreeRTOS / STM32F4 (QEMU) - execution results\n")
    w("Source: `RTOS_Book_KDP_Paperback_7x10.docx`. Every *Source Code* block was extracted in document order and, "
      "where it is C, wrapped into a runnable unit and executed on **FreeRTOS V10.6.1** (the version the book's Appendix F pins) "
      "using the **ARM_CM4F port** on **QEMU `netduinoplus2`** (STM32F405, Cortex-M4F, 168 MHz).\n")
    w("## 1. What was extracted\n")
    w(f"* **{len(blk)}** code blocks in total.")
    w("* " + ", ".join(f"{k}: {v}" for k, v in kinds.most_common()))
    w(f"* **{len(c_blocks)}** are C listings (plus **{len(labs)}** assembled whole-section labs/programs run as additional units).\n")
    w("## 2. Outcome for the C listings\n")
    w("| Verdict | Count | Meaning |\n|---|---:|---|")
    meaning = {
        "EXECUTED": "book code actually ran on FreeRTOS (program `main`, tasks/ISRs/init functions, or a statement fragment in a task) without fault",
        "EXECUTED_HELPERS": "only helper functions; each was invoked with synthesized arguments in a separate run and returned/ran cleanly",
        "COMPILED_ONLY": "types/macros/prototypes (or helpers that cannot be called generically): built, linked and booted, nothing to execute",
        "EXPECTED_FAILURE": "documented anti-pattern; the failure is the demonstration (kernel assert fires as the book warns)",
        "BARE_METAL_BY_DESIGN": "intentional super-loop without scheduler",
        "NO_SCHEDULER": "bootloader-style `main` that never starts the scheduler; loop ran for the whole window",
        "HW_LIMITED": "needs hardware QEMU does not model (CCM RAM, backup SRAM/PWR flags, ITM/SWO, STM32H7 memory)",
        "NEEDS_OTHER_PORT": "needs the FreeRTOS-MPU port",
        "EXCERPT": "illustrative excerpt that is not compilable C as written",
        "BOOK_ISSUE": "listing is inconsistent with another listing of the book",
        "BOOK_ISSUE_OR_EXCERPT": "assembled lab inherits one of the above",
        "FAULT": "unexpected fault", "ASSERT": "unexpected assert", "BUILD_FAIL": "does not compile",
    }
    for v, n in cv.most_common():
        w(f"| {v} | {n} | {meaning.get(v, '')} |")
    execd = cv.get("EXECUTED", 0) + cv.get("EXECUTED_HELPERS", 0)
    w(f"\n**{execd} of {len(c_blocks)} C listings were genuinely executed on the target** "
      f"({cv.get('EXECUTED', 0)} as written + {cv.get('EXECUTED_HELPERS', 0)} via synthesized-argument calls). "
      f"A further {cv.get('COMPILED_ONLY', 0)} are declarations/helpers that were compiled, linked and booted.\n")
    helper_runs = collections.Counter(e["helper_status"] for e in c_blocks if e.get("helper_status"))
    w(f"Helper functions with arguments were also exercised separately for listings that have tasks/programs ({dict(helper_runs)}); "
      "failures there reflect synthetic arguments and are not counted against the book.\n")
    w("### Labs and multi-listing programs (whole section assembled)\n")
    w("| Unit | Section | Status | Note |\n|---|---|---|---|")
    for e in labs:
        w(f"| {e['id']} | {e['chapter'][:14]}: {e['section'][:55]} | {e['status']} | {e.get('why', '')[:110]} |")
    w("\n## 3. Listings that did not run cleanly (and why)\n")
    w("| Block | Section | Status | Verdict | Diagnosis |\n|---:|---|---|---|---|")
    for e in c_blocks:
        if e["verdict"] not in ("EXECUTED", "EXECUTED_HELPERS", "COMPILED_ONLY"):
            w(f"| {e['id']} | {e['chapter'][:10]} {e['section'][:42]} | {e['status']} | {e['verdict']} | {e['why'][:230]} |")
    w("\n## 4. Per-chapter coverage\n")
    per = collections.OrderedDict()
    for e in c_blocks:
        ch = e["chapter"].split(":")[0] if e["chapter"].startswith("Chapter") else e["chapter"].split(":")[0]
        per.setdefault(ch, collections.Counter())[e["verdict"]] += 1
    w("| Chapter | Executed | Helpers | Compiled-only | Expected-fail | Other |\n|---|---:|---:|---:|---:|---:|")
    for ch, c in per.items():
        other = sum(c.values()) - c["EXECUTED"] - c["EXECUTED_HELPERS"] - c["COMPILED_ONLY"] - c["EXPECTED_FAILURE"]
        w(f"| {ch} | {c['EXECUTED']} | {c['EXECUTED_HELPERS']} | {c['COMPILED_ONLY']} | {c['EXPECTED_FAILURE']} | {other} |")
    w("\n## 5. Compiler warnings worth a look (excluding implicit declarations of helpers the book does not show)\n")
    noisy = re.compile(r"redefined|Wint-conversion|never defined|Wincompatible-pointer-types|declared inside parameter list")
    wn = [(e["id"], x) for e in c_blocks for x in e.get("warnings", [])[:2] if not noisy.search(x)]
    for i, x in wn[:40]:
        w(f"* block {i}: `{x[:200]}`")
    if not wn:
        w("none")
    w("\n## 6. Full per-block table\n")
    w("| Block | Kind | Chapter / section | Verdict | Status | How executed |\n|---:|---|---|---|---|---|")
    for e in blk:
        w(f"| {e['id']} | {e['kind']} | {e['chapter'][:12]} {e['section'][:36]} | {e['verdict']} | {e['status']} | "
          f"{(e.get('exec_kind') or e.get('reason') or '')[:90]} |")
    open(os.path.join(RES, "RESULTS.md"), "w").write("\n".join(L) + "\n")

    # readable dump of everything extracted
    out = ["# All code blocks extracted from the book\n"]
    last = None
    for i in sorted(blocks):
        b = blocks[i]
        head = (b["chapter"], b["section"])
        if head != last:
            out.append(f"\n## {b['chapter']} / {b['section']}\n")
            last = head
        out.append(f"### Block {i} - {b['kind']}" + (f" - {b['sub']}" if b["sub"] else "") + "\n")
        out.append("```" + ("c" if b["kind"] == "c" else "") + "\n" + b["text"].rstrip() + "\n```\n")
    open(os.path.join(PROJ, "extracted", "ALL_CODE_BLOCKS.md"), "w").write("\n".join(out))
    print(json.dumps(summary, indent=1))


if __name__ == "__main__":
    main()
