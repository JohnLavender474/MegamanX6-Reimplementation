import sys
sys.path.insert(0, sys.argv[0].rsplit("/", 1)[0] or ".")
import ram
r = ram.load(sys.argv[1])
base, n, size = int(sys.argv[2], 0), int(sys.argv[3]), int(sys.argv[4]) if len(sys.argv) > 4 else 4
for i in range(n):
    a = base + size * i
    v = ram.word(r, a) & ((1 << (8 * size)) - 1)
    print(i, hex(v))
