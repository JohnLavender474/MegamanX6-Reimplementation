import re, sys
val = sys.argv[1]
for path in sys.argv[2:]:
    lines = open(path).read().splitlines()
    for i, l in enumerate(lines):
        m = re.search(rf'addiu (\w+),zero,{val}$', l)
        if not m: continue
        reg = m.group(1)
        win = lines[max(0, i-6):i+6]
        if any(re.search(r'lbu? \w+,1\(\w+\)$', x) for x in win) and any(re.search(rf'(beq|bne) \w+,{reg},|(beq|bne) {reg},\w+,', x) for x in win):
            print(path, l[:8])
