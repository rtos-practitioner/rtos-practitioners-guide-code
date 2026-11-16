#!/usr/bin/env python3
"""Extract every 'Source Code' block from the book (.docx) in document order.

Outputs (under extracted/):
  blocks/NNN_<kind>.txt    raw text of each block
  index.json / index.csv   metadata: idx, chapter, section, sub, kind, lines, file
  blocks.json              metadata + full text (used by the test generator)
and platform/generated/book_FreeRTOSConfig_D9.h  (Appendix D.9, verbatim)
"""
import csv
import json
import os
import re
import sys

import docx
from docx.oxml.ns import qn
from docx.text.paragraph import Paragraph

ROOT = os.path.dirname(os.path.abspath(__file__))
PROJ = os.path.dirname(ROOT)
DEFAULT_DOCX = os.path.join(os.path.dirname(PROJ), "RTOS_Book_KDP_Paperback_7x10.docx")


def para_text(p_el):
    out = []
    for node in p_el.iter():
        if node.tag == qn("w:t"):
            out.append(node.text or "")
        elif node.tag == qn("w:br"):
            out.append("\n")
        elif node.tag == qn("w:tab"):
            out.append("\t")
    return "".join(out)


DIAGRAM_RE = re.compile(r"[┌└├│─┐┘┬┴┼▲▼]|^\s*\+[-=]{3,}|-{3,}>|<-{3,}", re.M)
C_SIGNAL_RE = re.compile(r"#include|;\s*$|^\s*\}\s*$", re.M)


FREERTOS_API = re.compile(r"\b(x(Task|Queue|Semaphore|Timer|Event|Stream|Message)\w*|v(Task|Queue|Semaphore|Timer)\w*|"
                          r"pdMS_TO_TICKS|pdTRUE|pdPASS|portMAX_DELAY|configASSERT|taskENTER_CRITICAL|FreeRTOS\.h)\b")
OTHER_RTOS_API = re.compile(r"\b(k_(thread|sem|mutex|msgq|timer|work|sleep|uptime|fifo)\w*|K_(MSEC|FOREVER|NO_WAIT|THREAD\w*|SEM\w*)\b|"
                            r"DEVICE_DT_GET|DT_NODELABEL|device_get_binding|sensor_(sample_fetch|channel_get|value_to_\w+|attr_set|trigger_set)\b|struct\s+sensor_value|"
                            r"ThreadCreate|ChannelCreate|ConnectAttach|MsgSend\w*|MsgReceive\w*|MsgReply|SIGEV_\w+|"
                            r"InterruptAttach\w*|pthread_\w+|tcsetattr|devctl|DCMD_\w+|resmgr_\w+|iofunc_\w+|O_RDWR|timer_(create|settime)|mq_(open|send|receive)|name_attach|"
                            r"CONFIG_[A-Z_]+=|west\b)")


def is_prose(s):
    lines = [l for l in s.splitlines() if l.strip()]
    if len(lines) < 2:
        return False
    codey = sum(1 for l in lines if re.search(r"[(){}=#\[\]<>]|^\s*(//|/\*)", l) and not re.match(r"^[A-Z][a-z].*[a-z]\.?\s*$", l))
    wordy = sum(1 for l in lines if len(re.findall(r"[A-Za-z]{3,}", l)) >= 5)
    return codey <= 0.2 * len(lines) and wordy >= 0.5 * len(lines)


def is_box_art(s):
    lines = [l for l in s.splitlines() if l.strip()]
    boxy = sum(1 for l in lines if re.match(r"^\s*(\+[-=+]+\+?|\|.*\|)\s*$", l))
    return len(lines) >= 3 and boxy >= 0.6 * len(lines)


def classify(text):
    s = text.strip()
    first = s.splitlines()[0] if s else ""
    if is_box_art(s):
        return "diagram"
    if OTHER_RTOS_API.search(s) and not FREERTOS_API.search(s):
        return "other-rtos"
    if is_prose(s):
        return "other"
    if DIAGRAM_RE.search(s) and not C_SIGNAL_RE.search(s):
        return "diagram"
    if re.match(r"^(\$ |west |cmake|make |openocd|arm-none|git |pip|python|qemu|gdb|apt|sudo|cd |mkdir|brew|export |ninja|\./|> test)", first) \
            or (re.match(r"^#\s", first) and re.search(r"^(gcc|cmake|make|arm-none|ninja|lcov|gcov)\b", s, re.M)):
        return "shell"
    if re.search(r"^\s*(cmake_minimum_required|project\(|add_executable|set\(CMAKE|target_|add_library|find_package)", s, re.M):
        return "cmake"
    if re.search(r"^\s*(MEMORY\s*\{|SECTIONS\s*\{|ENTRY\()", s, re.M) or re.search(r"^\s*\.\w+\s*(\(NOLOAD\))?\s*:\s*\{", s, re.M) \
            or re.search(r"^\s*_Min_\w+\s*=", s, re.M):
        return "linker"
    if re.match(r"^\s*(;|\s)*(LDR|ADDS|STR|MOV|BL|PUSH|POP)\s", s) and not re.search(r"\w\(", s) or re.search(r"\bEQU\b", s):
        return "asm"
    if first.startswith(("{", "#!/bin/bash", "# .github", "name:")) or "cortex-debug" in s:
        return "config-other"
    if re.search(r"#include|#define|\b(void|int|static|typedef|struct|enum|BaseType_t|TaskHandle_t|QueueHandle_t|SemaphoreHandle_t|TimerHandle_t|uint\d+_t|volatile|const)\b[^\n]*[;{(]", s):
        return "c"
    if re.search(r"[=;{}]", s) and re.search(r"\w+\s*\(.*\)", s) and re.search(r";\s*$|\{\s*$", s, re.M):
        return "c"
    # statement-only fragments such as `g_count++;` or `SCB->SCR |= X;` (or a bare `if (...)` excerpt)
    if (re.search(r";\s*(/\*.*\*/|//.*)?\s*$", s, re.M) or re.match(r"^if\s*\(", s)) \
            and not re.search(r"^\s*(\w+\s*)?[-=]{3,}|Σ|≈|^\s*\w+\s*=.*\b(mAh|mA|Hz|seconds)\b", s, re.M):
        return "c"
    return "other"


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_DOCX
    d = docx.Document(path)
    chapter = section = sub = ""
    blocks = []
    for el in d.element.body.iterchildren():
        if el.tag == qn("w:p"):
            p = Paragraph(el, d)
            st = p.style.name
            if st == "Heading 1":
                chapter, section, sub = p.text.strip(), "", ""
            elif st == "Heading 2":
                section, sub = p.text.strip(), ""
            elif st == "Heading 3":
                sub = p.text.strip()
            if st == "Source Code":
                blocks.append(dict(chapter=chapter, section=section, sub=sub, text=para_text(el)))
    out_dir = os.path.join(PROJ, "extracted")
    bdir = os.path.join(out_dir, "blocks")
    os.makedirs(bdir, exist_ok=True)
    for f in os.listdir(bdir):
        os.remove(os.path.join(bdir, f))
    index = []
    for i, b in enumerate(blocks, 1):
        b["idx"] = i
        b["kind"] = classify(b["text"])
        b["lines"] = b["text"].count("\n") + 1
        fn = f"{i:03d}_{b['kind']}.txt"
        with open(os.path.join(bdir, fn), "w", encoding="utf-8") as fh:
            fh.write(b["text"] + "\n")
        b["file"] = f"blocks/{fn}"
        index.append({k: v for k, v in b.items() if k != "text"})
    with open(os.path.join(out_dir, "index.json"), "w", encoding="utf-8") as fh:
        json.dump(index, fh, indent=1, ensure_ascii=False)
    with open(os.path.join(out_dir, "index.csv"), "w", newline="", encoding="utf-8") as fh:
        w = csv.DictWriter(fh, fieldnames=["idx", "kind", "chapter", "section", "sub", "lines", "file"])
        w.writeheader()
        for r in index:
            w.writerow({k: r[k] for k in w.fieldnames})
    with open(os.path.join(out_dir, "blocks.json"), "w", encoding="utf-8") as fh:
        json.dump(blocks, fh, indent=1, ensure_ascii=False)

    gen = os.path.join(PROJ, "platform", "generated")
    os.makedirs(gen, exist_ok=True)
    for b in blocks:
        if b["text"].lstrip().startswith("#ifndef FREERTOS_CONFIG_H") and "D.9" in b["sub"]:
            with open(os.path.join(gen, "book_FreeRTOSConfig_D9.h"), "w", encoding="utf-8") as fh:
                fh.write(b["text"] + "\n")
    kinds = {}
    for b in blocks:
        kinds[b["kind"]] = kinds.get(b["kind"], 0) + 1
    print(f"extracted {len(blocks)} blocks -> {out_dir}")
    print("kinds:", kinds)


if __name__ == "__main__":
    main()
