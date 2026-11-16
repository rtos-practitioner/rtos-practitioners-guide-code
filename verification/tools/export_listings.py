#!/usr/bin/env python3
"""Write every code listing of the book to listings/<chapter>/ as a stand-alone source file.

Input : extracted/blocks.json (from tools/extract_blocks.py)
Status: results/results.json if present (same extraction), otherwise the reference run in
        results/reference-run-*/ matched by listing text.
Output: ../listings/<chapter-dir>/NN-<name>.<ext>, a README.md per chapter, and listings/README.md
"""
import json
import os
import re
import shutil
from glob import glob

PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = os.path.dirname(PROJ)
OUT = os.path.join(REPO, "listings")

BOOK = "Real-Time Operating Systems: A Practitioner's Guide to FreeRTOS, QNX, and Zephyr"
TARGET = "QEMU netduinoplus2 (STM32F405, Cortex-M4F), FreeRTOS V10.6.1 ARM_CM4F port"
SKIP_KINDS = {"diagram", "other"}

VERDICTS = {
    "EXECUTED": "executed",
    "EXECUTED_HELPERS": "executed (helper functions called with test arguments)",
    "COMPILED_ONLY": "compiled, linked and booted (declarations only)",
    "HW_LIMITED": "compiled; needs real hardware (not modelled by QEMU)",
    "NEEDS_OTHER_PORT": "needs the FreeRTOS-MPU port (not built by the harness)",
    "EXPECTED_FAILURE": "anti-pattern: fails exactly as the book describes",
    "EXCERPT": "illustrative excerpt",
    "BARE_METAL_BY_DESIGN": "bare-metal super-loop (no scheduler by design)",
    "NO_SCHEDULER": "bootloader-style program (no scheduler by design)",
    "BOOK_ISSUE": "fixed in the book after the reference run",
    "NOT_CODE": "not run (host command / other RTOS / configuration)",
}

NAME_RE = re.compile(
    r"^\s*(?:/\*|//|#|;)\s*(?:In\s+)?([\w./-]+\.(?:c|h|ld|s|S|py|yml|yaml|txt|cmake|sh|json))\b")


def norm(text):
    return re.sub(r"\s+", " ", text).strip()


def slug(text, maxlen=64):
    s = re.sub(r"[^a-z0-9]+", "-", text.lower()).strip("-")
    return s[:maxlen].rstrip("-") or "listing"


def chapter_dir(chapter):
    m = re.match(r"Chapter (\d+):\s*(.*)", chapter)
    if m:
        return f"ch{int(m.group(1)):02d}-{slug(m.group(2), 60)}"
    m = re.match(r"Appendix ([A-H]):\s*(.*)", chapter)
    if m:
        return f"appendix-{m.group(1).lower()}-{slug(m.group(2), 60)}"
    return "00-" + slug(chapter or "front-matter", 40)


def extension(kind, text, name):
    if name:
        return os.path.splitext(name)[1]
    first = text.lstrip().splitlines()[0] if text.strip() else ""
    if kind == "c":
        return ".h" if re.search(r"^\s*#ifndef\s+\w+_H\b", text, re.M) else ".c"
    if kind == "other-rtos":
        if re.match(r"^\s*(west|pip|python3?|brew|arm-none|cmake|sudo|export)\b", first):
            return ".sh"
        return ".conf" if re.match(r"^\s*CONFIG_", first) else ".c"
    if kind == "config-other":
        if first.startswith("{"):
            return ".json"
        if first.startswith("#!"):
            return ".sh"
        return ".yml" if re.search(r"^\s*(name|on|jobs):", text, re.M) else ".txt"
    return {"shell": ".sh", "cmake": ".cmake", "linker": ".ld", "asm": ".s"}.get(kind, ".txt")


def header(lines, ext):
    if ext == ".json":
        return None
    if ext in (".sh", ".py", ".yml", ".yaml", ".cmake", ".txt", ".conf"):
        return "\n".join("# " + l if l else "#" for l in lines) + "\n"
    return "/*\n" + "\n".join(" * " + l if l else " *" for l in lines) + "\n */\n"


def load_status(blocks):
    fresh = os.path.join(PROJ, "results", "results.json")
    if os.path.exists(fresh):
        res = {r["id"]: r for r in json.load(open(fresh, encoding="utf-8"))}
        return {b["idx"]: res.get(b["idx"]) for b in blocks}, "this repository's latest run"
    refs = sorted(glob(os.path.join(PROJ, "results", "reference-run-*")))
    if not refs:
        return {}, None
    ref = refs[-1]
    ref_blocks = json.load(open(os.path.join(ref, "blocks.json"), encoding="utf-8"))
    res = {r["id"]: r for r in json.load(open(os.path.join(ref, "results.json"), encoding="utf-8"))}
    by_text = {norm(b["text"]): res.get(b["idx"]) for b in ref_blocks}
    return {b["idx"]: by_text.get(norm(b["text"])) for b in blocks}, os.path.basename(ref)


def describe(r):
    if r is None:
        return "changed since the reference run - re-run tools/run_suite.py"
    v = r.get("verdict", "")
    return VERDICTS.get(v, v.lower().replace("_", " "))


def main():
    blocks = json.load(open(os.path.join(PROJ, "extracted", "blocks.json"), encoding="utf-8"))
    status, source = load_status(blocks)
    if os.path.isdir(OUT):
        shutil.rmtree(OUT)
    os.makedirs(OUT)

    chapters = {}
    for b in blocks:
        if b["kind"] in SKIP_KINDS:
            continue
        d = chapter_dir(b["chapter"])
        chapters.setdefault(d, {"title": b["chapter"] or "Front matter", "items": []})["items"].append(b)

    index_rows = []
    for d, ch in chapters.items():
        os.makedirs(os.path.join(OUT, d))
        rows = []
        for n, b in enumerate(ch["items"], 1):
            text = b["text"].rstrip() + "\n"
            first = text.lstrip().splitlines()[0]
            m = NAME_RE.match(first)
            name = os.path.basename(m.group(1)) if m else ""
            ext = extension(b["kind"], text, name)
            if name:
                stem = os.path.splitext(name)[0]
            else:
                sec_no = b["section"].split(" ")[0] if b["section"] else ""
                sub = b["sub"] or b["section"]
                if re.fullmatch(r"(\S+ )?Code Examples?", sub):
                    sub = b["section"]
                stem = slug(sub if re.match(r"^[\dA-H]+\.\d", sub) else f"{sec_no} {sub}")
                tag = re.match(r"^\s*/\*\s*(WRONG|CORRECT|BAD|GOOD|FIXED)\b", text)
                if tag:
                    stem += "-" + tag.group(1).lower()
            fname = f"{n:02d}-{stem}{ext}"
            where = " > ".join(x for x in (b["section"], b["sub"]) if x)
            verdict = describe(status.get(b["idx"])) if source else "not yet run"
            hdr = header([BOOK, b["chapter"] or "Front matter", where,
                          f"Verification: {verdict}", "SPDX-License-Identifier: MIT"], ext)
            if hdr and text.startswith("#!"):
                shebang, rest = text.split("\n", 1)
                body = shebang + "\n" + hdr + rest
            else:
                body = (hdr or "") + text
            with open(os.path.join(OUT, d, fname), "w", encoding="utf-8", newline="\n") as fh:
                fh.write(body)
            rows.append((fname, where, verdict))

        with open(os.path.join(OUT, d, "README.md"), "w", encoding="utf-8", newline="\n") as fh:
            fh.write(f"# {ch['title']}\n\n")
            fh.write("Listings in the order they appear in the chapter. Most are excerpts that rely on "
                     "declarations from earlier listings or from the lab project; see the repository README.\n\n")
            fh.write("| File | Section | Verification |\n|---|---|---|\n")
            for fname, where, verdict in rows:
                fh.write(f"| [{fname}]({fname}) | {where} | {verdict} |\n")
        index_rows.append((d, ch["title"], len(rows)))

    with open(os.path.join(OUT, "README.md"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("# Code listings by chapter\n\n")
        if source:
            fh.write(f"Verification status from {source} on {TARGET}.\n\n")
        fh.write("| Chapter | Listings |\n|---|---:|\n")
        for d, title, count in index_rows:
            fh.write(f"| [{title}]({d}/) | {count} |\n")
    print(f"wrote {sum(r[2] for r in index_rows)} listings in {len(index_rows)} folders -> {OUT}")


if __name__ == "__main__":
    main()
