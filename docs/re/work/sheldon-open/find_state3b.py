import re, sys
for path in sys.argv[1:]:
    lines = open(path).read().splitlines()
    for i, l in enumerate(lines):
        m = re.search(r'(?:addiu|ori) (\w+),zero,(?:3|0x3)$', l)
        if not m: continue
        reg = m.group(1)
        for j in range(i, min(i + 12, len(lines))):
            mm = re.search(rf'(sb|sh|sw) {reg},4\((\w+)\)$', lines[j])
            if mm:
                print(path, lines[j][:8], mm.group(1), mm.group(2)); break
            if re.search(rf' {reg},', lines[j]) and j > i and not re.search(r'^\S+: \S+  (sb|sh|sw) ', lines[j]): break
