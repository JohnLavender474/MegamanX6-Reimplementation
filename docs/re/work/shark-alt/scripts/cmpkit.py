"""Compare two kit code.txt files: instruction count and the lines that differ after masking
relocations (jumps, branches, lui and the low half of lui pairs)."""
import re, sys
def load(p):
    out = []
    for line in open(p):
        m = re.match(r'([0-9a-f]{8}): ([0-9a-f]{8})  (\S+)\s*(.*)', line)
        if m:
            op, args = m.group(3), m.group(4).split(';')[0].strip()
            out.append((op, args))
    return out
def norm(op, args):
    if op in ('j', 'jal') or op.startswith('b'):
        args = re.sub(r'0x[0-9a-f]+', 'T', args)
    if op == 'lui':
        args = re.sub(r'0x[0-9a-f]+', 'H', args)
    if op in ('addiu', 'lw', 'sw', 'lh', 'lhu', 'sh', 'lb', 'lbu', 'sb', 'ori'):
        pass
    return op + ' ' + args
a, b = load(sys.argv[1]), load(sys.argv[2])
print(len(a), len(b))
diff = 0
for i, (x, y) in enumerate(zip(a, b)):
    if norm(*x) != norm(*y):
        diff += 1
        if diff <= int(sys.argv[3]) if len(sys.argv) > 3 else 20:
            print(i, x, y)
print('differ', diff)
