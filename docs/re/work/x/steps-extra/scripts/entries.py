import re, sys
for path in sys.argv[1:]:
    L = open(path).read().splitlines()
    fn = None
    print("==", path)
    out = {}
    for i, l in enumerate(L):
        if l.startswith("; ===="):
            fn = l.split()[2]
        m = re.search(r"\bsb (\w+),5\((\w+)\)", l)
        if not m: continue
        reg = m.group(1)
        for j in range(i, max(0, i - 12), -1):
            m2 = re.search(rf"addiu {reg},zero,(-?\d+)\b", L[j])
            if m2:
                n = int(m2.group(1))
                if n >= 32: out.setdefault(n, []).append(f"{fn}@{l[:8]}")
                break
    for n in sorted(out): print(n, out[n])
