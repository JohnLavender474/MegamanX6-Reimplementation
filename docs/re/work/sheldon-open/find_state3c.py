import re, sys
DEF = re.compile(r'^\S+: \S+  (\w+) (\w+),')
def lastdef(lines, i, reg, lim=40):
    for j in range(i - 1, max(i - lim, 0), -1):
        t = lines[j]
        if re.search(r'addiu sp,sp,-', t): return None, j
        m = DEF.match(t)
        if m and m.group(2) == reg and m.group(1) not in ('sb', 'sh', 'sw', 'beq', 'bne', 'swl', 'swr', 'jr', 'mult', 'div', 'multu', 'divu'):
            return t, j
    return None, None
for path in sys.argv[1:]:
    lines = open(path).read().splitlines()
    for i, l in enumerate(lines):
        m = re.search(r'  sb (\w+),4\((\w+)\)$', l)
        if not m: continue
        rx, ry = m.groups()
        d, j = lastdef(lines, i, rx)
        if d is None or not re.search(r'addiu \w+,zero,3$', d): continue
        dy, k = lastdef(lines, i, ry, 80)
        print(path, l[:8], ry, '<-', dy[20:] if dy else '(arg/unknown)')
