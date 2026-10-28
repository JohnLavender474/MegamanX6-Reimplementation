import re, sys, os
from collections import defaultdict
root = os.path.expanduser('~/mmx-data/x6/re')
files = [os.path.join(d, f) for d, _, fs in os.walk(root) for f in fs if f.endswith('.c') and 'reconstructed' in d]
defs = {}
calls = defaultdict(set)
where = {}
fn_re = re.compile(r'^(?:static\s+)?(?:[A-Za-z_][\w\s\*]*?)\b([A-Za-z_]\w*)\s*\(([^;]*?)\)\s*\{', re.M)
for path in files:
    src = open(path).read()
    for m in fn_re.finditer(src):
        name = m.group(1)
        if name in ('if', 'while', 'for', 'switch'):
            continue
        start = m.end()
        depth, i = 1, start
        while depth and i < len(src):
            depth += {'{': 1, '}': -1}.get(src[i], 0)
            i += 1
        body = src[start:i]
        key = (path, name)
        defs[key] = body
        for c in re.findall(r'\b([A-Za-z_]\w*)\s*\(', body):
            calls[key].add(c)
byname = defaultdict(list)
for (p, n) in defs:
    byname[n].append(p)
def same_file(p, n):
    return (p, n) in defs
seen = set()
for (p, n), cs in list(calls.items()):
    if n in cs:
        print('SELF', os.path.relpath(p, root), n)
    for c in cs:
        if c != n and same_file(p, c) and n in calls.get((p, c), ()) and (p, c, n) not in seen:
            seen.add((p, n, c))
            print('MUTUAL', os.path.relpath(p, root), n, '<->', c)
g = defaultdict(set)
for (p, n), cs in list(calls.items()):
    g[n] |= {c for c in cs if c in byname}
cycles = set()
for a in list(g):
    for b in g[a]:
        if b == a: continue
        for c in g[b]:
            if c == a: cycles.add(tuple(sorted((a, b))))
            elif a in g.get(c, ()): cycles.add(tuple(sorted((a, b, c))))
for cyc in sorted(cycles):
    print('CYCLE', cyc, [sorted({os.path.relpath(p, root).split('/reconstructed')[0] for p in byname[x]}) for x in cyc])
