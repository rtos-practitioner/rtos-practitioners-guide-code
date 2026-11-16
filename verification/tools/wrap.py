"""wrap.py - turn a book code block into a compilable, runnable FreeRTOS "unit".

Execution shapes (decided by parsing the block, then confirmed by compiling):
  program   block defines main()               -> main renamed book_main, harness calls it
  library   block defines functions            -> glue spawns tasks / calls init functions / pends ISRs
  fragment  block is bare statements           -> wrapped in a function, executed in a runner task
  mixed     definitions + a usage snippet      -> definitions at file scope, snippet run in the runner
  decls     types / macros / prototypes        -> compile + link + boot only

Anything the listing assumes but does not show is supplied automatically and reported:
  * kernel objects behind handle variables (queues, semaphores, ...), HAL handles, constants, structs
  * headers/sources that exist as *other listings* of the book (project files), else empty stub headers
  * helper functions defined nowhere in the book (weak link-time stubs)
"""
import os
import re

import rtos_qemu as q

PRELUDE = '#include "unit_prelude.h"\n'

# ----------------------------------------------------------------------------
# lexical helpers
# ----------------------------------------------------------------------------
_COMMENT_OR_STR = re.compile(
    r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', re.S)


def mask(text):
    """Blank out comments / string+char literals / preprocessor lines (offsets + newlines preserved)."""
    s = _COMMENT_OR_STR.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)
    out, cont = [], False
    for line in s.split("\n"):
        if cont or line.lstrip().startswith("#"):
            cont = line.rstrip().endswith("\\")
            out.append(re.sub(r"[^\n]", " ", line))
        else:
            out.append(line)
    return "\n".join(out)


CONTROL = ("if", "for", "while", "do", "switch", "else", "return", "break", "continue", "goto", "case")
TYPE_START = re.compile(r"^(typedef|struct|enum|union|extern)\b")
DECL_START = re.compile(r"^\s*(?:(?:static|extern|const|volatile|register|unsigned|signed|struct|enum|union|inline)\s+)*"
                        r"(?!(?:return|goto|else|case|break|continue|sizeof)\b)[A-Za-z_]\w*[\s\*]+(?:const\s+)?[A-Za-z_]\w*")


def split_items(text):
    """Split into top-level items: list of dict(start, end, kind, sig). kind in
    func | type | decl | stmt | ctl.  (preprocessor lines are not items.)"""
    m = mask(text)
    n = len(m)
    items, depth, start, i = [], 0, 0, 0
    brace_open_at = None
    while i < n:
        ch = m[i]
        if ch == "{":
            if depth == 0:
                brace_open_at = i
            depth += 1
        elif ch == "}":
            depth = max(0, depth - 1)
            if depth == 0:
                sig = m[start:brace_open_at].strip() if brace_open_at is not None else ""
                first = re.match(r"[A-Za-z_]\w*", sig)
                fw = first.group(0) if first else ""
                is_func = sig.endswith(")") and fw not in CONTROL and "=" not in sig.split("(")[0] \
                    and not TYPE_START.match(sig)
                is_ctl = fw in CONTROL
                if is_func or is_ctl:
                    j = i + 1
                    tail = m[j:].lstrip()
                    if is_ctl and (re.match(r"else\b", tail) or (fw == "do" and re.match(r"while\b", tail))):
                        i += 1
                        continue
                    lead = len(m[start:brace_open_at]) - len(m[start:brace_open_at].lstrip())
                    items.append(dict(start=start, sig_start=start + lead, end=i + 1,
                                      kind="func" if is_func else "ctl", sig=sig))
                    start = i + 1
        elif ch == ";" and depth == 0:
            seg = m[start:i].strip()
            if seg:
                if TYPE_START.match(seg) and not re.match(r"^extern\b.*\(.*\)", seg):
                    kind = "type"
                elif re.match(r"^(" + "|".join(CONTROL) + r")\b", seg) or re.match(r"^do\b", seg):
                    kind = "stmt"
                elif DECL_START.match(seg) and not re.match(r"^\s*\w+\s*\(", seg):
                    # a declaration; one with a call/arrow in its initializer cannot live at file scope
                    kind = "decl-dyn" if re.search(r"=\s*[^=]*(\w\s*\(|->|\.\w)", seg) and "sizeof" not in seg \
                        and not re.search(r"=\s*(\{|\(\s*\w+\s*\)\s*\{)", seg) \
                        and not re.search(r"=\s*(pdMS_TO_TICKS|\()", seg) else "decl"
                else:
                    kind = "stmt"
                lead = len(m[start:i]) - len(m[start:i].lstrip())
                items.append(dict(start=start, sig_start=start + lead, end=i + 1, kind=kind, sig=seg))
            start = i + 1
        i += 1
    rest = m[start:].strip()
    if rest:
        items.append(dict(start=start, sig_start=start, end=n, kind="stmt", sig=rest))
    return items


def top_level_funcs(text):
    funcs = []
    for it in split_items(text):
        if it["kind"] != "func":
            continue
        sig = it["sig"]
        j, d = len(sig) - 1, 0
        while j >= 0:
            if sig[j] == ")":
                d += 1
            elif sig[j] == "(":
                d -= 1
                if d == 0:
                    break
            j -= 1
        if j <= 0:
            continue
        mname = re.search(r"([A-Za-z_]\w*)\s*$", sig[:j])
        if not mname:
            continue
        name = mname.group(1)
        funcs.append(dict(name=name, params=sig[j + 1:-1].strip(), static=bool(re.search(r"\bstatic\b", sig[:j])),
                          ret=sig[:mname.start()].strip(), start=it["sig_start"], name_off=mname.start(),
                          body=text[it["start"]:it["end"]]))
    return funcs


def shape_of(text):
    items = split_items(text)
    funcs = top_level_funcs(text)
    if any(f["name"] == "main" for f in funcs):
        return "program", funcs
    kinds = [it["kind"] for it in items]
    first_dyn = next((k for k, it in enumerate(items) if it["kind"] in ("stmt", "ctl", "decl-dyn")), None)
    if funcs:
        return ("mixed" if first_dyn is not None and any(
            items[k]["kind"] in ("stmt", "ctl") for k in range(first_dyn, len(items))) else "library"), funcs
    if not items:
        return "decls", funcs
    if first_dyn is None:
        return "decls", funcs
    return "fragment", funcs


def split_head_tail(text):
    """Candidate (head, tail) splits for definitions followed by a usage snippet."""
    items = split_items(text)
    cands = []
    idx = next((k for k, it in enumerate(items) if it["kind"] in ("stmt", "ctl", "decl-dyn")), None)
    if idx is None:
        return cands
    # walk back over contiguous plain declarations that probably belong to the snippet
    for back in range(0, 4):
        k = idx - back
        if k < 0 or (back and items[k]["kind"] not in ("decl", "decl-dyn")):
            break
        cut = items[k]["start"]
        cands.append((text[:cut], text[cut:]))
    return cands


def rename_duplicate_functions(text):
    """Listings sometimes show a 'bad' and a 'good' version under the same name: keep both."""
    funcs = top_level_funcs(text)
    seen = {}
    edits = []
    for f in funcs:
        c = seen[f["name"]] = seen.get(f["name"], 0) + 1
        if c > 1:
            edits.append((f["start"] + f["name_off"], f["name"], f"{f['name']}__v{c}"))
    for pos, old, new in sorted(edits, reverse=True):
        text = text[:pos] + new + text[pos + len(old):]
    return text, [e[1] for e in edits]


# ----------------------------------------------------------------------------
# automatic stubbing
# ----------------------------------------------------------------------------
HAL_HANDLES = [
    (r"^huart\w*|^husart\w*", "UART_HandleTypeDef"), (r"^hspi\w*", "SPI_HandleTypeDef"),
    (r"^hi2c\w*", "I2C_HandleTypeDef"), (r"^htim\w*", "TIM_HandleTypeDef"),
    (r"^hdma\w*", "DMA_HandleTypeDef"), (r"^hcan\w*", "CAN_HandleTypeDef"),
    (r"^hadc\w*", "ADC_HandleTypeDef"), (r"^hiwdg\w*", "IWDG_HandleTypeDef"),
    (r"^hwwdg\w*", "WWDG_HandleTypeDef"), (r"^hrng\w*", "RNG_HandleTypeDef"),
    (r"^hrtc\w*", "RTC_HandleTypeDef"), (r"^hcrc\w*", "CRC_HandleTypeDef"),
    (r"^hdac\w*", "DAC_HandleTypeDef"),
]
_enum_counter = [0]
HANDLE_TYPES = ("QueueHandle_t", "SemaphoreHandle_t", "TimerHandle_t", "EventGroupHandle_t",
                "StreamBufferHandle_t", "MessageBufferHandle_t", "QueueSetHandle_t", "TaskHandle_t")


def handle_ctor(typ, name):
    """C expression creating a default kernel object for handle `name` of type `typ`."""
    if typ == "QueueHandle_t":
        return f"xQueueCreate(8, sizeof(uint32_t))"
    if typ == "QueueSetHandle_t":
        return "xQueueCreateSet(16)"
    if typ == "SemaphoreHandle_t":
        if re.search(r"Recursive", name):
            return "xSemaphoreCreateRecursiveMutex()"
        if re.search(r"Mutex|Lock|Mtx", name, re.I):
            return "xSemaphoreCreateMutex()"
        if re.search(r"Count", name):
            return "xSemaphoreCreateCounting(10, 0)"
        return "xSemaphoreCreateBinary()"
    if typ == "TimerHandle_t":
        return 'xTimerCreate("dflt", pdMS_TO_TICKS(100), pdTRUE, NULL, harness_noop_timer_cb)'
    if typ == "EventGroupHandle_t":
        return "xEventGroupCreate()"
    if typ == "StreamBufferHandle_t":
        return "xStreamBufferCreate(256, 1)"
    if typ == "MessageBufferHandle_t":
        return "xMessageBufferCreate(256)"
    return None


INT_LIKE_TYPE = re.compile(r"(Code|Type|State|Status|Mode|Id|ID|Result|Error|Err|Level|Kind|Phase|Prio|Priority|Flags|Mask)_t$")
FUNC_NAME = re.compile(r"^(test_|cmd_|handler_|isr_|cb_|on_)\w+$|_(test|cmd|fn|func|handler|cb|callback|task|isr|entry|hook)$|"
                       r"^(v|prv)[A-Z]\w*$|^(v|prv|x)[A-Z]\w*(Task|Callback|Handler|Hook|Func|Entry|Cb)$")


def type_kind(T, src):
    """'struct' or 'int' for a type the book never defines (decided from how it is used)."""
    for v in re.findall(rf"\b{T}\s*\**\s*(\w+)\s*(?:\[[^\]]*\])?\s*[;,=)]", src):
        if re.search(rf"\b{v}\s*(?:\[[^\]]*\])?\s*(?:\.|->)\s*\w", src):
            return "struct"
    if re.search(rf"\(\s*{T}\s*\*\s*\)[^;]*->|sizeof\s*\(\s*{T}\s*\)\s*\*", src):
        return "struct"
    return "int" if INT_LIKE_TYPE.search(T) else "struct"


def looks_like_type(name, src):
    return bool(re.search(r"_t$", name)) and (
        re.search(rf"sizeof\s*\(\s*{name}\s*\)|\(\s*{name}\s*\*?\s*\)|\b{name}\s+\**\w+\s*[;=,)\[]", src))


def stub_for_variable(name, src, stubs):
    """Declaration text for an undeclared identifier (or None to give up)."""
    if looks_like_type(name, src):
        stubs.types.setdefault(name, None if type_kind(name, src) == "int" else {})
        return ""
    for pat, typ in HAL_HANDLES:
        if re.match(pat, name):
            return f"static {typ} {name};"
    if re.fullmatch(r"[A-Z][A-Z0-9_]*", name):
        _enum_counter[0] += 1
        if re.search(rf"\b{name}\s*\(", src):
            return f"#define {name}(...) 0"
        val = _enum_counter[0] % 7 + 1
        if re.search(r"(QUEUE|LEN|DEPTH|COUNT|NUM|SIZE|MAX|ITEMS)", name):
            val = 4
        if re.search(r"(STACK|HEAP)", name):
            val = 256
        if re.search(r"(PRIO|PRIORITY)", name):
            val = 2
        if re.search(r"(PERIOD|_MS|TIMEOUT|DELAY|INTERVAL)", name):
            val = 100
        if re.search(r"(BIT|FLAG|EVT|EVENT|MASK)", name):
            val = 1 << (_enum_counter[0] % 8)
        return f"#define {name} ({val})"
    if FUNC_NAME.search(name) and not re.search(rf"\b{name}\s*(\.|->|\[)", src):
        return f"void {name}();"
    arr = re.search(rf"\b{name}\s*\[", src)
    suffix = "[64]" if arr else ""
    # objects accessed through members: stub as a struct whose members are discovered from diagnostics
    if re.search(rf"\b{name}\s*(?:\[[^\]]*\])?\s*(?:\.|->)\s*\w", src):
        T = f"{name}_stub_t"
        stubs.types.setdefault(T, {})
        if arr:
            return f"static {T} {name}[16];"
        if re.search(rf"\b{name}\s*->", src) and not re.search(rf"\b{name}\s*\.", src):
            return f"static {T} {name}_obj; static {T} *{name} = &{name}_obj;"
        return f"static {T} {name};"
    typ = handle_type_by_usage(name, src)
    if not typ and (re.search(rf"sizeof\s*\(\s*{name}\s*\)", src) or re.search(r"(Buf|Buffer|Array|Data)$", name)) \
            and not arr and not name.startswith(("x", "ul")):
        return f"static uint8_t {name}[64];"
    if not typ and re.search(r"Set\b", name) and name.startswith("x"):
        typ = "QueueSetHandle_t"
    elif not typ and re.search(r"^x\w*(Queue|Q)\b|^x\w*Queue\w*|^q[A-Z]\w*", name):
        typ = "QueueHandle_t"
    elif not typ and re.search(r"Mutex|Lock|Mtx", name):
        typ = "SemaphoreHandle_t"
    elif not typ and re.search(r"Sem|Semaphore|Signal", name) and name.startswith(("x", "s")):
        typ = "SemaphoreHandle_t"
    elif not typ and re.search(r"Timer", name) and name.startswith("x") and not re.search(r"Period|Count", name):
        typ = "TimerHandle_t"
    elif not typ and re.search(r"EventGroup|^xEvents?\b|^xEG", name):
        typ = "EventGroupHandle_t"
    elif not typ and re.search(r"MsgBuf|MessageBuf|^xMB", name):
        typ = "MessageBufferHandle_t"
    elif not typ and re.search(r"StreamBuf|^xSB", name):
        typ = "StreamBufferHandle_t"
    if typ == "TaskHandle_t":
        return f"static TaskHandle_t {name}{suffix};"
    if typ and not suffix:
        return (f"static {typ} {name};\n"
                f"static void __attribute__((constructor)) stubinit_{name}(void) {{ {name} = {handle_ctor(typ, name)}; }}")
    if re.search(r"Task|Handle$", name) and name.startswith(("x", "h")):
        return f"static TaskHandle_t {name}{suffix};"
    if re.search(r"Time|Period|Frequency|Delay|Tick|Timeout|Wait|Wake", name) and name.startswith(("x", "ul")):
        return f"static TickType_t {name}{suffix}" + ("" if suffix else " = pdMS_TO_TICKS(100)") + ";"
    if name.startswith(("pc", "psz")):
        return f"static char {name}_buf[64] = \"stub\"; static char *{name} = {name}_buf;"
    if name.startswith("puc"):
        return f"static uint8_t {name}buf[64]; static uint8_t *{name} = {name}buf;"
    if re.match(r"^p[A-Za-z]*[A-Z]", name) and not suffix:
        return f"static uint8_t {name}_buf[64]; static void *{name} = {name}_buf;"
    if re.match(r"^f[A-Z]", name):
        return f"static volatile float {name}{suffix};"
    if re.match(r"^d[A-Z]", name):
        return f"static volatile double {name}{suffix};"
    if re.match(r"^x[A-Z]", name):
        return f"static BaseType_t {name}{suffix};"
    if re.match(r"^e[A-Z]", name):
        return f"static int {name}{suffix};"
    t = "volatile uint8_t" if re.search(r"^(uc|b[A-Z]|u8)", name) else "volatile uint32_t"
    return f"static {t} {name}{suffix};"


USAGE_APIS = [
    (r"x?Queue(?:Send\w*|Receive\w*|Peek\w*|Overwrite\w*|Reset|Add\w*|Remove\w*)|uxQueue\w+|vQueue\w+", "QueueHandle_t"),
    (r"xSemaphore(?:Take|Give)\w*|uxSemaphoreGetCount|vSemaphoreDelete", "SemaphoreHandle_t"),
    (r"xTimer(?:Start|Stop|Reset|Delete|ChangePeriod|IsTimerActive|GetPeriod|GetExpiryTime)\w*|pvTimerGetTimerID|vTimerSetTimerID|pcTimerGetName", "TimerHandle_t"),
    (r"xEventGroup\w+|vEventGroupDelete", "EventGroupHandle_t"),
    (r"x(?:Stream)Buffer(?:Send|Receive|Reset|IsEmpty|IsFull|BytesAvailable|SpacesAvailable)\w*", "StreamBufferHandle_t"),
    (r"x(?:Message)Buffer(?:Send|Receive|Reset|IsEmpty|IsFull|SpacesAvailable)\w*", "MessageBufferHandle_t"),
    (r"xTask(?:Notify\w*|AbortDelay|ResumeFromISR|GetStackHighWaterMark)|vTask(?:Suspend|Resume|Delete|PrioritySet|GetInfo)|uxTaskPriorityGet|eTaskGetState|pcTaskGetName|uxTaskGetStackHighWaterMark", "TaskHandle_t"),
]


def handle_type_by_usage(name, src):
    """Kernel-object type inferred from the FreeRTOS API the identifier is passed to as first argument."""
    for api, typ in USAGE_APIS:
        if re.search(rf"\b(?:{api})\s*\(\s*&?\s*{name}\s*[,)]", src):
            return typ
    return None


def member_type(name, src):
    if re.search(rf"(?:\.|->)\s*{name}\s*\(", src):
        return f"uintptr_t (*{name})()"
    if re.search(rf"(?:\.|->)\s*{name}\s*\[", src):
        return f"uint8_t {name}[64]"
    if re.match(r"^f[A-Z]", name):
        return f"float {name}"
    if re.match(r"^d[A-Z]", name):
        return f"double {name}"
    if re.match(r"^x(Timestamp|Time|Tick)", name):
        return f"TickType_t {name}"
    if re.match(r"^uc", name):
        return f"uint8_t {name}"
    if re.match(r"^us", name):
        return f"uint16_t {name}"
    if re.match(r"^(pv|p)[A-Z]", name):
        return f"void *{name}"
    return f"uint32_t {name}"


class Stubs:
    def __init__(self):
        self.undefs = set()     # FreeRTOS function-like macros the listing re-declares as prototypes
        self.renames = set()    # names the listing (re)defines that clash with port/startup symbols
        self.vars = {}          # name -> declaration text
        self.types = {}         # typename -> {member: decl} (struct) | None (integer-like)
        self.funcs = {}         # helper functions returning a struct (implicit `int` would not compile)
        self.includes = {}      # header name -> content (project listings, or empty stub)
        self.project_srcs = []  # (name, text) project .c listings pulled in with their headers

    def render(self):
        out = ["/* ---- auto-generated stubs for symbols the listing assumes but does not show ---- */"]
        out += [f"#undef {m}" for m in sorted(self.undefs)]
        out += [f"#define {m} book_{m}" for m in sorted(self.renames)]
        for t, members in self.types.items():
            if members is None:
                out.append(f"typedef int32_t {t};")
            else:
                body = "".join(f"    {d};\n" for _, d in sorted(members.items())) or "    uint32_t _stub;\n"
                out.append(f"typedef struct {{\n{body}}} {t};")
        out.extend(v for v in self.vars.values() if v)
        out.extend(self.funcs.values())
        return "\n".join(out) + "\n"

    def names(self):
        n = set(self.vars) | set(self.types) | set(self.funcs) | {"#undef " + m for m in self.undefs} | \
            {f"rename {m}" for m in self.renames}
        for h, c in self.includes.items():
            n.add(f"include {h}" + (" (book listing)" if c.strip() != "/* stub */" else " (empty stub)"))
        return sorted(n)

    def empty(self):
        return not (self.vars or self.types or self.undefs or self.renames or self.funcs)


ERR_UNDECL = re.compile(r"error: '(\w+)' undeclared")
ERR_UNKNOWN_TYPE = re.compile(r"error: unknown type name '(\w+)'")
ERR_NOMEMBER = re.compile(r"error: '([\w ]+?)'(?: \{aka [^}]*\})? has no member named '(\w+)'")
ERR_NOTSTRUCT = re.compile(r"error: request for member '(\w+)' in something not a structure or union")
ERR_NOINCLUDE = re.compile(r"fatal error: ([\w./+-]+): No such file or directory")


ERR_INT_TO_STRUCT = re.compile(r"chk\.c:(\d+):\d+: error: incompatible types when (?:assigning to|initializing) type '([\w ]+)'(?: \{aka[^}]*\})? from type 'int'")
NOT_USER_FUNC = re.compile(r"^(x|v|ux|ul|us|pd|port|task|config|HAL_|NVIC_|__|if$|for$|while$|switch$|sizeof$|return$)")


def update_stubs(stubs, err, src, registry=None, self_text=None, compiled=None):
    """Grow `stubs` from compiler diagnostics. Returns True if anything new was added."""
    registry = registry or {}
    changed = False
    if compiled:
        lines = compiled.split("\n")
        for ln, typ in ERR_INT_TO_STRUCT.findall(err):
            line = lines[int(ln) - 1] if 0 < int(ln) <= len(lines) else ""
            for fn in re.findall(r"\b([A-Za-z_]\w*)\s*\(", line):
                if not NOT_USER_FUNC.match(fn) and fn not in stubs.funcs:
                    stubs.funcs[fn] = f"static {typ} {fn}() {{ {typ} t; memset(&t, 0, sizeof t); return t; }}"
                    changed = True
                    break
    for hdr in ERR_NOINCLUDE.findall(err):
        base = os.path.basename(hdr)
        if hdr in stubs.includes:
            continue
        if base in registry:
            stubs.includes[hdr] = registry[base]["text"]
            stem = os.path.splitext(base)[0]
            impl = registry.get(stem + ".c")
            if impl and impl["text"].strip() == (self_text or "").strip():
                impl = None                       # the listing *is* that source file
            if impl and (stem + ".c") not in [n for n, _ in stubs.project_srcs]:
                stubs.project_srcs.append((stem + ".c", impl["text"]))
        else:
            stubs.includes[hdr] = "/* stub */\n"
        changed = True
    for m in re.findall(r"in expansion of macro '(\w+)'", err) + re.findall(r'macro "(\w+)" passed \d+ arguments', err):
        # only when the listing *declares* the API as a prototype ("QueueHandle_t xQueueCreate( ... );"), never for calls
        if m not in stubs.undefs and re.match(r"^[a-z]+[A-Z]", m) and \
                re.search(rf"^\s*(?:extern\s+)?(?:const\s+)?[A-Za-z_]\w*[\s\*]+{m}\s*\(", src, re.M):
            stubs.undefs.add(m)
            changed = True
    for m in re.findall(r"error: (?:redefinition|conflicting types) (?:of|for) '(\w+)'", err):
        if m not in stubs.renames and len(re.findall(rf"^[^\n#]*\b{m}\s*\([^;]*\)\s*\n?\s*\{{", src, re.M)) <= 1 \
                and re.search(rf"\b{m}\b", src):
            stubs.renames.add(m)
            changed = True
    for name in ERR_UNDECL.findall(err):
        if name in stubs.vars or name in stubs.types:
            continue
        before = len(stubs.types)
        decl = stub_for_variable(name, src, stubs)
        if decl is not None:
            stubs.vars[name] = decl
            changed = True
    for t in ERR_UNKNOWN_TYPE.findall(err):
        if t not in stubs.types:
            stubs.types[t] = None if type_kind(t, src) == "int" else {}
            changed = True
    if ERR_NOTSTRUCT.search(err):
        for t, members in list(stubs.types.items()):          # an "int-like" stub that is used as a struct
            if members is None and type_kind(t, src + "\n" + t) == "struct":
                stubs.types[t] = {}
                changed = True
    for t, mem in ERR_NOMEMBER.findall(err):
        t = t.replace("struct ", "").strip()
        if t in stubs.types and stubs.types[t] is not None and mem not in stubs.types[t]:
            stubs.types[t][mem] = member_type(mem, src)
            changed = True
    for m in ERR_NOTSTRUCT.findall(err):
        pass
    return changed


# ----------------------------------------------------------------------------
# glue: how the code is exercised
# ----------------------------------------------------------------------------
TASK_PARAM = re.compile(r"^\s*(?:const\s+)?void\s*\*\s*(\w*)\s*$")
SKIP_NAMES = re.compile(r"(IRQHandler|_Handler$|Hook$|Callback$|^HAL_|^main$|^Reset|^Default|^SysTick|^SVC|^PendSV|"
                        r"^NMI|^HardFault|^MemManage|^BusFault|^UsageFault|^DebugMon|^vApplication|^Error_Handler|"
                        r"^SystemClock|^SystemInit|^_|^assert_failed|^vAssertCalled|^book_)")
INIT_NAMES = re.compile(r"(^|_)(init|setup|start|create|startup|app_main|begin)$|^v(Start|Create|Init|Setup)\w*$|"
                        r"^prv(Setup|Init|Start|Create)\w*$|(^|_)(init|setup)(_|$)", re.I)
RENAME_ALWAYS = ("Reset_Handler", "SystemInit", "SystemCoreClockUpdate", "Default_Handler")


def irq_names():
    hdr = os.path.join(q.DEPS, "cmsis_device_f4", "Include", "stm32f407xx.h")
    return set(re.findall(r"^\s*(\w+_IRQn)\s*=", open(hdr).read(), re.M))


IRQN = irq_names()


SCALAR_T = re.compile(r"^(?:const\s+|volatile\s+|unsigned\s+|signed\s+)*(?:u?int(?:8|16|32|64)_t|size_t|ssize_t|BaseType_t|UBaseType_t|"
                      r"TickType_t|EventBits_t|uintptr_t|intptr_t|bool|_Bool|char|short|int|long|float|double|"
                      r"\w+_t|\w+Type|enum\s+\w+)(?:\s+(?:long|int|short))*$")


def synth_args(params):
    """C argument list for a helper function, or None when it cannot be called generically."""
    p = params.strip()
    if p in ("", "void"):
        return []
    args = []
    for one in split_params(p):
        one = one.strip()
        if "..." in one:
            return None
        if "(*" in one:
            args.append("(void *)harness_noop_cb")
        elif "*" in one or re.search(r"\[[^\]]*\]\s*$", one):
            args.append("(void *)harness_scratch")
        else:
            m = re.match(r"^(.*?)(?:\s+|\*)(\w+)\s*$", one)
            typ = (m.group(1) if m else one).strip()
            if not typ or not SCALAR_T.match(typ):
                return None                                   # struct by value etc.
            args.append("1.0f" if re.search(r"\b(float|double)\b", typ) else "1")
    return args


def split_params(p):
    out, depth, cur = [], 0, ""
    for ch in p:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    out.append(cur)
    return out


def build_glue(funcs):
    tasks, inits, isrs, calls, timers = [], [], [], [], []
    for f in funcs:
        n, p = f["name"], f["params"]
        if n == "main":
            continue
        if n == "NMI_Handler":
            isrs.append("__NMI__")
            continue
        if re.match(r"^\s*TimerHandle_t\s+\w*\s*$", p) and re.search(r"Callback|_cb$|Timer", n):
            timers.append(n)                      # exercised by a real software timer
            continue
        if n.endswith("_IRQHandler") and f"{n[:-len('Handler')]}n" in IRQN:
            isrs.append(n[:-len("Handler")] + "n")
            continue
        if SKIP_NAMES.search(n):
            continue
        tp = TASK_PARAM.match(p)
        if tp and (re.search(r"Task|task|Thread|Loop|Worker", n) or tp.group(1).startswith("pv")):
            tasks.append(n)
        elif p in ("", "void") and re.search(r"(Task|_task|Thread|_thread)$", n):
            tasks.append(n)
        elif p in ("", "void") and INIT_NAMES.search(n):
            inits.append(n)
        elif "vTaskStartScheduler" not in f.get("body", ""):
            args = synth_args(p)
            if args is not None:
                calls.append((n, args))
    return tasks, inits, isrs, calls, timers


def handle_globals(text):
    """File-scope kernel-object handle variables defined by the listing."""
    found = []
    for it in split_items(text):
        if it["kind"] not in ("decl", "decl-dyn"):
            continue
        m = re.match(r"^\s*(?:static\s+|volatile\s+|extern\s+)*(" + "|".join(HANDLE_TYPES) + r")\s+(.*)$", it["sig"], re.S)
        if not m or re.match(r"^\s*extern\b", it["sig"]):
            continue
        for d in re.split(r",", m.group(2)):
            nm = re.match(r"\s*\*?\s*(\w+)\s*(?:=\s*(?:NULL|0))?\s*$", d)
            if nm:
                found.append((m.group(1), nm.group(1)))
    return found


def glue_text(tasks, inits, isrs, handles, has_fragment, stub_task_handles=(), calls=(), timers=()):
    lines = ["", "/* ---- harness glue: how this listing is exercised on target ---- */",
             "static uint8_t harness_scratch[512] __attribute__((aligned(8)));",
             "static void harness_noop_cb(void) { }"]
    for n, args in calls:
        lines.append(f"static void harness_call_{n}(void *p) {{ (void)p; (void){n}({', '.join(args)}); "
                     f"printf(\"[HARNESS] helper {n}() returned\\r\\n\"); for (;;) {{ vTaskDelay(portMAX_DELAY); }} }}")
    lines += ["static void unit_runner(void)", "{"]
    task_handles = [n for t, n in handles if t == "TaskHandle_t"] + list(stub_task_handles)
    for typ, name in handles:
        ctor = handle_ctor(typ, name)
        if ctor:
            lines.append(f"    if ({name} == NULL) {{ {name} = {ctor}; }}   /* default kernel object */")
    for n in inits:
        lines.append(f"    {n}();")
    for n, _ in calls:
        lines.append(f"    xTaskCreate(harness_call_{n}, \"call_{n[:10]}\", 512, NULL, 2, NULL);")
    for n in timers:
        lines.append(f"    {{ TimerHandle_t t_{n} = xTimerCreate(\"{n[:10]}\", pdMS_TO_TICKS(50), pdTRUE, NULL, {n}); "
                     f"if (t_{n}) {{ xTimerStart(t_{n}, 0); }} }}")
    for n in tasks:
        core = re.sub(r"(Task|_task)$", "", re.sub(r"^(v|prv|x)(?=[A-Z])", "", n))
        h = next((t for t in task_handles if len(core) >= 3 and core.lower() in t.lower()), None)
        lines.append(f"    xTaskCreate(harness_task_adapter, \"{n[:15]}\", 512, (void *){n}, 2, {'&' + h if h else 'NULL'});")
    for h in task_handles:
        lines.append(f"    if ({h} == NULL) {{ {h} = xTaskGetCurrentTaskHandle(); }}   /* notifications target the runner */")
    if isrs:
        lines.append("    vTaskDelay(pdMS_TO_TICKS(20));")
        for rep in range(3):
            for irq in isrs:
                if irq == "__NMI__":
                    lines.append("    SCB->ICSR = SCB_ICSR_NMIPENDSET_Msk;")
                    continue
                if rep == 0:
                    lines.append(f"    NVIC_SetPriority({irq}, 6); NVIC_EnableIRQ({irq});")
                lines.append(f"    NVIC_SetPendingIRQ({irq});")
            lines.append("    vTaskDelay(pdMS_TO_TICKS(20));")
    if has_fragment:
        lines.append("    unit_fragment();")
    lines += ["}", "void unit_setup(void) { harness_spawn_runner(unit_runner, 1024, 3); }"]
    return "\n".join(lines) + "\n"


def deadline_timer(source):
    """A timer the code under test leaves alone: harness deadline uses TIM3, else TIM5, else TIM2."""
    code = re.sub(r"/\*.*?\*/|//[^\n]*", "", source, flags=re.S)
    for n in (3, 5, 2):
        if not re.search(rf"\bTIM{n}\b|TIM{n}_IRQ|TIM{n}_BASE", code):
            return n
    return 0            # every spare timer is used by the listing: harness uses a top-priority task instead


def undef_config(code):
    """Config-style listings redefine macros FreeRTOSConfig.h already defines: #undef first (no warnings)."""
    out = []
    for line in code.split("\n"):
        m = re.match(r"^\s*#\s*define\s+(config\w+|INCLUDE_\w+|port\w+|tick\w+)\b", line)
        if m and not line.rstrip().endswith("\\"):
            out.append(f"#undef {m.group(1)}")
        out.append(line)
    return "\n".join(out)


DWT_REMAP = [(re.compile(r"0x[Ee]0001000(?:[uU][lL]?)?"), "0x40000800UL /* DWT_CTRL -> TIM4_CR1 (QEMU has no DWT) */"),
             (re.compile(r"0x[Ee]0001004(?:[uU][lL]?)?"), "0x40000824UL /* DWT_CYCCNT -> TIM4_CNT (QEMU has no DWT) */")]


def remap_dwt(code):
    """QEMU lacks the DWT unit; listings that poke 0xE0001000/4 by raw address are re-pointed at the TIM4 overlay."""
    for rx, repl in DWT_REMAP:
        code = rx.sub(repl, code)
    return code


def prerename(code):
    """Startup/system symbols that a listing may (pseudo-)define: keep the real ones."""
    pre = [n for n in RENAME_ALWAYS if re.search(rf"\b{n}\s*\([^;]*\)\s*\{{", code)]
    return pre


def assemble(mode, code, stubs, context="", fragment_code=None):
    """mode: program | library | fragment | mixed.  For mixed, `code` is the head and fragment_code the tail."""
    code = remap_dwt(code)
    context = remap_dwt(context)
    stubs.renames.update(prerename(code + context))
    parts = [PRELUDE, stubs.render() if not stubs.empty() or stubs.types else ""]
    for name, text in stubs.project_srcs:
        parts.append(f"/* ---- project listing: {name} ---- */\n{remap_dwt(undef_config(text))}\n")
    if context:
        parts.append("/* ---- context: preceding listings from the same section ---- */\n" + context + "\n")
    parts.append("/* ---- book listing ---- */")
    if mode == "program":
        parts.append("#define main book_main\n" + undef_config(code) + "\n#undef main\n")
    elif mode == "fragment":
        parts.append("static void unit_fragment(void)\n{\n" + code + "\n}\n"
                     "void unit_setup(void) { harness_spawn_runner(unit_fragment, 1024, 3); }\n")
    elif mode == "mixed":
        parts.append(undef_config(code) + "\nstatic void unit_fragment(void)\n{\n" + remap_dwt(fragment_code) + "\n}\n")
    else:
        parts.append(undef_config(code) + "\n")
    return "\n".join(parts)


def syntax_check(src, workdir, stubs=None, tag="chk"):
    os.makedirs(workdir, exist_ok=True)
    path = os.path.join(workdir, f"{tag}.c")
    with open(path, "w") as fh:
        fh.write(src)
    inc = write_includes(stubs.includes if stubs else {}, workdir)
    r = q.run([q.CC, *q.CFLAGS, *inc, "-fsyntax-only", "-w", path])
    return r.returncode == 0, r.stderr


def write_includes(includes, workdir):
    d = os.path.join(workdir, "stubinc")
    flags = []
    if includes:
        for name, content in includes.items():
            p = os.path.join(d, name)
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, "w") as fh:
                fh.write(content if content.endswith("\n") else content + "\n")
        flags.append(f"-I{d}")
    unity = os.path.join(q.DEPS, "Unity", "src")
    if os.path.isdir(unity):
        flags.append(f"-I{unity}")
    return flags


def _try(mode, text, tail, context, workdir, registry, base_src, self_text=None):
    stubs = Stubs()
    src = assemble(mode, text, stubs, context, tail)
    ok, err = syntax_check(src, workdir, stubs)
    rounds = 0
    while not ok and rounds < 14:
        rounds += 1
        if not update_stubs(stubs, err, base_src, registry, self_text, src):
            break
        src = assemble(mode, text, stubs, context, tail)
        ok, err = syntax_check(src, workdir, stubs)
    return ok, src, stubs, err


def make_unit(block, ctx_levels, workdir, registry=None):
    """Try shapes/contexts until the listing compiles."""
    registry = registry or {}
    text = block["text"].rstrip() + "\n"
    shape, funcs = shape_of(text)
    text, dup = rename_duplicate_functions(text)
    funcs = top_level_funcs(text)
    attempts = {"program": ["program"], "library": ["library"], "mixed": ["mixed", "library"],
                "fragment": ["fragment"], "decls": ["library", "fragment"]}[shape]
    first_err = ""
    for ctx_name, context in ctx_levels:
        for mode in attempts:
            variants = [(text, None)]
            if mode == "mixed":
                variants = [(h, t) for h, t in split_head_tail(text)] or []
            for head, tail in variants:
                base_src = (head + "\n" + (tail or "") + "\n" + context)
                ok, src, stubs, err = _try(mode, head, tail, context, workdir, registry, base_src, block["text"])
                if not first_err:
                    first_err = err
                if not ok:
                    continue
                gfuncs = top_level_funcs(head)
                exec_kind = {"program": "program (main)", "fragment": "fragment in runner task"}.get(mode, "")
                helper_src, helper_names = None, ""
                if mode in ("library", "mixed"):
                    tasks, inits, isrs, calls, timers = build_glue(gfuncs)
                    handles = handle_globals(head + "\n" + context)
                    stub_th = [m.group(1) for d in stubs.vars.values() for m in [re.match(r"static TaskHandle_t (\w+)", d or "")] if m]
                    glued = helper_src = None
                    # faithful run: tasks / init / ISR / usage snippet exactly as the listing defines them
                    if tasks or inits or isrs or mode == "mixed":
                        cand = src + glue_text(tasks, inits, isrs, handles, mode == "mixed", stub_th, [], [])
                        if syntax_check(cand, workdir, stubs, tag="glue")[0]:
                            glued = cand
                    # separate run: every other function invoked with synthesized arguments
                    if calls or timers:       # timer callbacks depend on state set up elsewhere: synthetic run
                        cand = src + glue_text([], inits, [], handles, False, stub_th, calls, timers)
                        if syntax_check(cand, workdir, stubs, tag="glueh")[0]:
                            helper_src = cand
                    helper_names = ",".join([n for n, _ in calls] + [f"timer:{t}" for t in timers]) if helper_src else ""
                    if glued:
                        src = glued
                        what = tasks + inits + [i for i in isrs if i != "__NMI__"] + (["NMI"] if "__NMI__" in isrs else []) \

                        exec_kind = ("definitions + usage snippet; " if mode == "mixed" else "") + "runs: " + ",".join(what) \
                            if what else "usage snippet in runner task"
                    else:
                        exec_kind = "declarations only" if shape == "decls" else "no-entry (helpers only; built+linked+booted)"
                    if shape == "decls" and not glued and not helper_src:
                        exec_kind = "declarations only"
                notes = []
                if dup:
                    notes.append("duplicate definitions kept as " + ",".join(f"{d}__vN" for d in sorted(set(dup))))
                return dict(ok=True, source=src, mode=mode, shape=shape, exec_kind=exec_kind, stubs=stubs.names(),
                            deadline_tim=deadline_timer(src), helper_source=helper_src, helper_names=helper_names,
                            ctx=ctx_name, errors="", includes=stubs.includes, notes=notes)
    return dict(ok=False, source=None, mode=None, shape=shape, exec_kind="", stubs=[], ctx="", errors=first_err,
                includes={}, notes=[], helper_source=None, helper_names="")


# ----------------------------------------------------------------------------
# link-time stubs for undefined references
# ----------------------------------------------------------------------------
UNDEF_RE = re.compile(r"undefined reference to [`'](\w+)'")


def _is_called(n, src):
    """True if `n` is *called* in the source (compile-time prototype stubs do not count)."""
    cleaned = re.sub(rf"^\s*(?:void|uintptr_t|static\s+\w+)\s+\**{n}\s*\([^)]*\)\s*;?\s*$", "", src, flags=re.M)
    return bool(re.search(rf"\b{n}\s*\(", cleaned))


def link_stub_source(undefs, src):
    out = ["/* ---- link-time stubs: symbols used by the listing but defined nowhere in the book ---- */",
           "#include <stdint.h>", '#include "FreeRTOS.h"', '#include "task.h"']
    for n in undefs:
        called = _is_called(n, src)
        if (re.search(r"task|thread", n, re.I) or re.search(r"(Function|Entry|Loop|Worker)$", n)) and not called:
            out.append(f"__attribute__((weak)) void {n}(void *p) {{ (void)p; for (;;) {{ vTaskDelay(pdMS_TO_TICKS(100)); }} }}")
        elif called or (re.search(r"^(v|prv|x|ux|ul|us|e|pv|pc|b|i|u|f)[A-Z]\w*$|_(cb|callback|handler)$", n, re.I)
                        and not re.search(rf"\b{n}\s*\[", src)):
            out.append(f"__attribute__((weak)) uintptr_t {n}(void) {{ return 0; }}")
        else:
            out.append(f"__attribute__((weak)) uint32_t {n}[64];")
    return "\n".join(out) + "\n"
