import re, sys
lines = open(sys.argv[1]).read().splitlines()
pat_li = re.compile(r'addiu (\w+),zero,3$')
for i, l in enumerate(lines):
    m = pat_li.search(l)
    if not m: continue
    reg = m.group(1)
    for j in range(i, min(i + 6, len(lines))):
        if re.search(rf'sb {reg},4\((\w+)\)$', lines[j]):
            base = re.search(rf'sb {reg},4\((\w+)\)$', lines[j]).group(1)
            if base not in ('a0', 's0', 's1', 's2') or True:
                print(lines[j][:8], base)
            break
