import re
import sys

LINE = re.compile(r'^([0-9a-f]{8}): ([0-9a-f]{8})  (\S+)\s*(.*?)(\s*;.*)?$')


def functions(path):
    out = []
    cur = None
    for line in open(path):
        if line.startswith('; ===='):
            cur = []
            out.append((line.strip(), cur))
            continue
        m = LINE.match(line.rstrip('\n'))
        if m and cur is not None:
            op, args = m.group(3), m.group(4)
            args = re.sub(r'0x800f[0-9a-f]{4}|0x8010[0-9a-f]{4}|-?\d{4,}', 'R', args)
            cur.append(op + ' ' + args)
    return out


a = functions(sys.argv[1])
b = functions(sys.argv[2])
print(len(a), 'functions in A,', len(b), 'in B')
for (ha, ia), (hb, ib) in zip(a, b):
    diff = sum(1 for x, y in zip(ia, ib) if x != y) + abs(len(ia) - len(ib))
    print(f"{diff:3d} diffs  {len(ia):4d}/{len(ib):4d}  {ha[:60]} | {hb[:60]}")
