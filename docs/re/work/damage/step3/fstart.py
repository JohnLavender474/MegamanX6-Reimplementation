import sys
sys.path.insert(0, sys.argv[0].rsplit("/", 1)[0] or ".")
import ram
r = ram.load(sys.argv[1])
for x in sys.argv[2:]:
    a = int(x, 0)
    b = a
    while not (ram.word(r, b) >> 16 == 0x27BD and (ram.word(r, b) & 0x8000)):
        b -= 4
    refs = []
    for lo, hi in ((0x80010000, 0x800E0000), (0x801EA000, 0x801F8000)):
        for c in range(lo, hi, 4):
            w = ram.word(r, c)
            if w == b or (w >> 26 in (2, 3) and (0x80000000 | ((w & 0x3FFFFFF) << 2)) == b):
                refs.append(hex(c))
    print(hex(a), "func", hex(b), "refs", refs)
