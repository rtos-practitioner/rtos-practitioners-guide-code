#!/usr/bin/env python3
"""run_suite.py - extract -> wrap -> build -> run every C listing of the book on FreeRTOS/QEMU(STM32F4).

usage: run_suite.py [--only 11,28,51] [--jobs 20] [--no-labs] [--rebuild]
Outputs: results/results.json, results/RESULTS.md, build/units/<unit>/{unit.c,unit.elf,run.log}
"""
import argparse
import collections
import json
import os
import re
import shutil
import sys
import time
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rtos_qemu as q      # noqa: E402
import wrap                # noqa: E402

PROJ = q.PROJ
RES = os.path.join(PROJ, "results")
UNITS = os.path.join(q.BUILD, "units")

NON_C_REASON = {
    "diagram": "ASCII diagram / table - not code",
    "other": "formula / prose / table - not code",
    "other-rtos": "QNX / Zephyr / POSIX API listing (comparison with another RTOS) - not FreeRTOS code",
    "shell": "host shell command (toolchain, git, cmake, west, ...) - not FreeRTOS code",
    "cmake": "CMake build script - build system, not FreeRTOS code",
    "linker": "linker-script fragment - build input, not FreeRTOS code",
    "asm": "assembly illustration - not a C listing",
    "config-other": "JSON/YAML/shell configuration - not FreeRTOS code",
}
ANTI_PATTERN_RE = re.compile(r"Pitfall|Anti-?Pattern|Common Mistake|Exercise \d+: Identify|WRONG|BAD\b|BROKEN|BUG\b", re.I)


def defines_main(text):
    """True only for a definition of main(); `extern int main(void);` declarations do not count."""
    return any(f["name"] == "main" for f in wrap.top_level_funcs(text))


def load_blocks():
    return json.load(open(os.path.join(PROJ, "extracted", "blocks.json")))


def file_registry(blocks):
    """Listings that announce a filename in their first comment ('/* src/moving_average.h */') act as project files."""
    reg = {}
    for b in blocks:
        m = re.match(r"\s*(?:/\*|//)\s*(?:File:\s*)?([\w./+-]+\.(?:c|h))\b", b["text"])
        if m and b["kind"] == "c" and os.path.basename(m.group(1)) != "FreeRTOSConfig.h":
            reg.setdefault(os.path.basename(m.group(1)), dict(text=b["text"], idx=b["idx"]))
    return reg


def run_window_ms(text):
    vals = [int(v) for v in re.findall(r"pdMS_TO_TICKS\(\s*(\d+)\s*\)", text) if int(v) <= 5000]
    big = max(vals) if vals else 0
    return int(min(5600, max(1000, big * 1.1 + 300)))


def ctx_text(blocks, idxs):
    return "\n\n".join(blocks[i - 1]["text"].rstrip() for i in idxs)


def generate(blocks, only=None, jobs=20, log=print, registry=None):
    """Three passes: standalone, +section context, +chapter context. Returns {idx: unit-dict}."""
    by_idx = {b["idx"]: b for b in blocks}
    cands = [b for b in blocks if b["kind"] == "c" and (not only or b["idx"] in only)]
    gen = {}
    wd = lambda b: os.path.join(UNITS, f"b{b['idx']:03d}")
    chk = lambda b, p: os.path.join(UNITS, f"b{b['idx']:03d}", f"chk{p}")

    def one(b, levels, p):
        return b["idx"], wrap.make_unit(b, levels, chk(b, p), registry)

    log(f"pass 1: standalone ({len(cands)} listings)")
    with ThreadPoolExecutor(jobs) as ex:
        for idx, u in ex.map(lambda b: one(b, [("none", "")], 1), cands):
            gen[idx] = u
    ok_tu = {i for i, u in gen.items() if u["ok"] and u["shape"] in ("library", "decls")
             and u["mode"] == "library" and not any(f["name"] == "main" for f in wrap.top_level_funcs(by_idx[i]["text"]))}
    for p, scope in ((2, "section"), (3, "chapter")):
        todo = [by_idx[i] for i, u in gen.items() if not u["ok"]]
        log(f"pass {p}: retry {len(todo)} failing listings with {scope} context")

        def levels_for(b):
            prev = [i for i in sorted(ok_tu) if i < b["idx"]]
            sec = [i for i in prev if by_idx[i]["chapter"] == b["chapter"] and by_idx[i]["section"] == b["section"]]
            chp = [i for i in prev if by_idx[i]["chapter"] == b["chapter"]]
            ids = sec if scope == "section" else chp
            if not ids:
                return []
            return [(f"{scope}:{','.join(map(str, ids[-12:]))}", ctx_text(blocks, ids[-12:]))]

        jobs_l = [(b, levels_for(b)) for b in todo]
        jobs_l = [(b, l) for b, l in jobs_l if l]
        with ThreadPoolExecutor(jobs) as ex:
            for idx, u in ex.map(lambda bl: one(bl[0], bl[1], p), jobs_l):
                if u["ok"]:
                    gen[idx] = u
    # pass 4: a listing that needed stubs for objects an earlier listing of the same chapter really defines
    # is retried with those definitions in front, so it runs against the book's own code, not my stubs
    def refine(b):
        u = gen[b["idx"]]
        names = [n.split()[-1] for n in u["stubs"] if not n.startswith(("include", "#undef", "rename"))]
        if not names:
            return b["idx"], None
        prev = [i for i in sorted(ok_tu) if i < b["idx"] and by_idx[i]["chapter"] == b["chapter"]]
        defs = [i for i in prev if any(re.search(rf"\b{re.escape(n)}\b", by_idx[i]["text"]) for n in names)]
        if not defs:
            return b["idx"], None
        sec = [i for i in prev if by_idx[i]["section"] == b["section"]]
        ids = sorted(set(defs[-8:]) | set(sec[-4:]))
        ctx = ctx_text(blocks, ids)
        nu = wrap.make_unit(b, [(f"refined:{','.join(map(str, ids))}", ctx)], chk(b, 4), registry)
        if nu["ok"] and len(nu["stubs"]) < len(u["stubs"]):
            return b["idx"], nu
        return b["idx"], None
    todo = [by_idx[i] for i, u in gen.items() if u["ok"] and u["stubs"] and u["ctx"] == "none"
            and by_idx[i]["kind"] == "c" and not defines_main(by_idx[i]["text"])]
    log(f"pass 4: refine {len(todo)} listings that relied on stubs for objects defined earlier in the book")
    with ThreadPoolExecutor(jobs) as ex:
        for idx, nu in ex.map(refine, todo):
            if nu:
                gen[idx] = nu
    return gen


def lab_units(blocks, gen, only=None):
    """Whole-section assemblies for labs and multi-listing programs."""
    secs = collections.OrderedDict()
    for b in blocks:
        if b["kind"] == "c":
            secs.setdefault((b["chapter"], b["section"]), []).append(b)
    out = []
    for (chap, sec), bl in secs.items():
        if len(bl) < 2:
            continue
        n_main = sum(1 for b in bl if defines_main(b["text"]))
        if n_main > 1:
            continue                      # several separate programs (e.g. M7 + M4 projects): not one unit
        has_main = n_main == 1
        if not (has_main or re.search(r"\bLab\b|Complete", sec)):
            continue
        parts = []
        for b in bl:
            shape, _ = wrap.shape_of(b["text"])
            if shape in ("fragment", "mixed") and not defines_main(b["text"]):
                if shape == "fragment":
                    continue
            parts.append(b["text"].rstrip())
        if len(parts) < 2:
            continue
        out.append(dict(idx=f"S{bl[0]['idx']:03d}", chapter=chap, section=sec, sub="(assembled section)",
                        kind="c", text="\n\n".join(parts), members=[b["idx"] for b in bl], lab=True))
    return out


def where(elf, pc):
    r = q.run([os.path.join(q.ARM_ROOT, "bin", "arm-none-eabi-addr2line"), "-f", "-C", "-e", elf, hex(pc & ~1)])
    lines = r.stdout.strip().split("\n")
    if len(lines) >= 2:
        return f"{lines[0]} ({os.path.basename(lines[1].split(':')[0])}:{lines[1].split(':')[-1]})"
    return hex(pc)


def build_and_run(name, source, run_ms, includes=None, wall_timeout=60, deadline_tim=3):
    wd = os.path.join(UNITS, name)
    os.makedirs(wd, exist_ok=True)
    csrc = os.path.join(wd, "unit.c")
    with open(csrc, "w") as fh:
        fh.write(source)
    inc = wrap.write_includes(includes or {}, wd)
    r = q.build_unit(csrc, name, run_ms=run_ms, workdir=wd, extra_cflags=inc, deadline_tim=deadline_tim)
    link_stubs = []
    if not r["ok"] and r["stage"] == "link":
        undefs = sorted(set(wrap.UNDEF_RE.findall(r["log"])))
        if undefs:
            stub_c = os.path.join(wd, "linkstubs.c")
            with open(stub_c, "w") as fh:
                fh.write(wrap.link_stub_source(undefs, source))
            sobj = os.path.join(wd, "linkstubs.o")
            c = q.run([q.CC, *q.CFLAGS, "-w", "-c", stub_c, "-o", sobj])
            if c.returncode == 0:
                r = q.build_unit(csrc, name, run_ms=run_ms, workdir=wd, extra_objs=[sobj], extra_cflags=inc, deadline_tim=deadline_tim)
                link_stubs = undefs
    warns = [re.sub(r"^.*?unit\.c:", "unit.c:", l) for l in (r.get("warnings") or "").split("\n")
             if "warning:" in l and "implicit declaration" not in l and "unused" not in l and "stub" not in l]
    res = dict(build=r["stage"], build_ok=r["ok"], link_stubs=link_stubs, warnings=warns[:6],
               build_log=r["log"][-1500:] if not r["ok"] else "")
    if r["ok"]:
        o = q.run_elf(r["elf"], wall_timeout=wall_timeout)
        res.update(status=o["status"], rc=o["rc"], wall=round(o["wall"], 2), output=o["output"][-2500:])
        m = re.search(r"tasks=(\d+) free_heap=(\d+) min_free_heap=(\d+) tick=(\d+) pc=0x([0-9a-f]+)", o["output"])
        if m:
            res.update(tasks=int(m.group(1)), min_free_heap=int(m.group(3)), ticks=int(m.group(4)))
            pc = int(m.group(5), 16)
            if res["status"] == "RAN_OK" and res["ticks"] == 0:
                res["status"] = "SCHED_NOT_STARTED"         # window elapsed but the scheduler never ran
            if res["ticks"] == 0 and pc:
                res["stuck_at"] = where(r["elf"], pc)
        with open(os.path.join(wd, "run.log"), "w") as fh:
            fh.write(o["output"])
    else:
        res.update(status="BUILD_FAIL" if r["stage"] != "link" else "LINK_FAIL", rc=None, wall=0, output="")
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", default="")
    ap.add_argument("--jobs", type=int, default=20)
    ap.add_argument("--no-labs", action="store_true")
    ap.add_argument("--rebuild", action="store_true")
    a = ap.parse_args()
    only = {int(x) for x in a.only.split(",") if x} or None
    t0 = time.time()
    q.build_platform(force=a.rebuild)
    blocks = load_blocks()
    os.makedirs(RES, exist_ok=True)
    registry = file_registry(blocks)
    gen = generate(blocks, only, a.jobs, registry=registry)

    entries = []   # one per block/unit
    for b in blocks:
        if only and b["idx"] not in only:
            continue
        e = dict(id=b["idx"], kind=b["kind"], chapter=b["chapter"], section=b["section"], sub=b["sub"],
                 lines=b["lines"], anti_pattern=bool(ANTI_PATTERN_RE.search(b["section"] + " " + b["sub"] + b["text"][:400])))
        if b["kind"] != "c":
            e.update(status="NOT_EXECUTABLE", reason=NON_C_REASON.get(b["kind"], b["kind"]))
        else:
            g = gen[b["idx"]]
            if not g["ok"]:
                e.update(status="BUILD_FAIL", reason="does not compile even with stubs/context", errors=g["errors"][-1200:],
                         shape=g["shape"])
            else:
                e.update(shape=g["shape"], mode=g["mode"], exec_kind=g["exec_kind"], stubs=g["stubs"], ctx=g["ctx"],
                         notes=g["notes"], _source=g["source"], _includes=g["includes"], run_ms=run_window_ms(b["text"]),
                         _dtim=g["deadline_tim"], _hsrc=g["helper_source"], helper_names=g["helper_names"])
        entries.append(e)
    if not a.no_labs and not only:
        for lu in lab_units(blocks, gen):
            res = wrap.make_unit(lu, [("none", "")], os.path.join(UNITS, f"lab{lu['idx']}", "chk"), registry)
            e = dict(id=lu["idx"], kind="lab", chapter=lu["chapter"], section=lu["section"], sub=lu["sub"],
                     lines=lu["text"].count("\n") + 1, members=lu["members"],
                     anti_pattern=bool(ANTI_PATTERN_RE.search(lu["section"])))
            if res["ok"]:
                e.update(shape=res["shape"], mode=res["mode"], exec_kind=res["exec_kind"], stubs=res["stubs"],
                         ctx=res["ctx"], notes=res["notes"], _source=res["source"], _includes=res["includes"],
                         run_ms=run_window_ms(lu["text"]), _dtim=res["deadline_tim"], _hsrc=res["helper_source"],
                         helper_names=res["helper_names"])
            else:
                e.update(status="BUILD_FAIL", reason="assembled section does not compile", errors=res["errors"][-1200:])
            entries.append(e)

    todo = [e for e in entries if "_source" in e]
    print(f"building + running {len(todo)} units on QEMU netduinoplus2 ...")

    def work(e):
        name = f"b{e['id']:03d}" if isinstance(e["id"], int) else f"lab{e['id']}"
        r = build_and_run(name, e["_source"], e["run_ms"], e.get("_includes"), deadline_tim=e.get("_dtim", 3))
        e.update(r)
        e["unit"] = name
        if e.get("_hsrc"):          # second run: helpers with synthesized arguments (kept separate from the faithful run)
            h = build_and_run(name + "h", e["_hsrc"], 1000, e.get("_includes"), deadline_tim=e.get("_dtim", 3))
            e["helper_status"] = h["status"]
            e["helper_output"] = h.get("output", "")[-1500:]
            e["helper_build_log"] = h.get("build_log", "")
        return e
    with ThreadPoolExecutor(a.jobs) as ex:
        list(ex.map(work, todo))
    for e in entries:
        e.pop("_source", None)
        e.pop("_includes", None)
        e.pop("_dtim", None)
        e.pop("_hsrc", None)
    json.dump(entries, open(os.path.join(RES, "results.json"), "w"), indent=1, ensure_ascii=False)
    summarize(entries, time.time() - t0)


def summarize(entries, elapsed):
    c = collections.Counter(e["status"] for e in entries if e["kind"] != "lab")
    print(f"\n== {len(entries)} entries in {elapsed:.0f}s ==")
    for k, v in sorted(c.items(), key=lambda kv: -kv[1]):
        print(f"  {k:18s} {v}")


if __name__ == "__main__":
    main()
