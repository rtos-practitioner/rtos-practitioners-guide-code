#!/usr/bin/env python3
"""dbg_block.py IDX [IDX...] - standalone wrap attempt for a block; prints errors / generated glue."""
import json, os, sys
HERE = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HERE)
import rtos_qemu as q, wrap, run_suite
B = json.load(open(os.path.join(q.PROJ, "extracted", "blocks.json")))
reg = run_suite.file_registry(B)
for i in map(int, sys.argv[1:]):
    b = B[i - 1]
    r = wrap.make_unit(b, [("none", "")], f"/tmp/dbg_{i}", reg)
    print(f"===== block {i} ok={r['ok']} shape={r['shape']} mode={r['mode']} exec={r['exec_kind']} stubs={r['stubs']}")
    if not r["ok"]:
        for l in [l for l in r["errors"].split("\n") if "error" in l][:5]:
            print("   ", l.replace(f"/tmp/dbg_{i}/chk.c", "chk.c"))
    elif "-v" in sys.argv:
        print(r["source"][-1500:])
