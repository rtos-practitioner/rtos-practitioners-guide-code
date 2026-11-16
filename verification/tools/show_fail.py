#!/usr/bin/env python3
import json,re,sys,os
R=json.load(open(os.path.join(os.path.dirname(__file__),'..','results','results.json')))
KERNEL=os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)),'..','deps','FreeRTOS-Kernel')).replace(os.sep,'/')+'/'
def first_err(s):
    for l in (s or '').split('\n'):
        m=re.search(r'(?:unit|chk)\.c:(\d+):\d+: (?:fatal )?error: (.*)', l)
        if m: return f"L{m.group(1)}: "+m.group(2)[:110]
    return (s or '')[-110:].replace('\n',' ')
for st in ('BUILD_FAIL','LINK_FAIL','FAULT','ASSERT','HANG','STACK_OVERFLOW','MALLOC_FAILED','NO_EXIT_RESET','SCHED_START_FAILED'):
    rows=[e for e in R if e['status']==st]
    if not rows: continue
    print('----',st,len(rows))
    for e in rows:
        if st=='BUILD_FAIL': print(e['id'], e.get('shape'), '|', first_err(e.get('errors')))
        elif st=='LINK_FAIL':
            ur=re.findall(r"undefined reference to [`'](\w+)'|multiple definition of [`'](\w+)'", e.get('build_log') or '')
            print(e['id'], [a or b for a,b in ur][:5])
        else:
            o=(e.get('output') or ''); line=[l for l in o.split('\n') if re.search('ASSERT|FAULT|HOOK|returned',l)][:2]
            print(e['id'], 'AP' if e.get('anti_pattern') else '  ', '|', e.get('exec_kind','')[:40], '|', ' / '.join(l[:120].replace(KERNEL,'') for l in line))
