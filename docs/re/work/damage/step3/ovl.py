import struct, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.mips import disassemble
root = Path.home() / "mmx-data/x6/extracted/retail/rock_bin"
n, base = sys.argv[1], int(sys.argv[2], 0)
d = (root / f"{n}.bin").read_bytes()
def w(a): return struct.unpack_from("<I", d, a - base)[0]
cmd = sys.argv[3]
if cmd == "dis":
    lo, hi = int(sys.argv[4], 0), int(sys.argv[5], 0)
    for a in range(lo, hi, 4): print(f"{a:08x}: {w(a):08x}  {disassemble(w(a), a)}")
elif cmd == "ref":
    t = int(sys.argv[4], 0)
    hi16 = (t + 0x8000) >> 16 & 0xFFFF; lo16 = t & 0xFFFF
    for a in range(base, base + len(d) - 3, 4):
        x = w(a)
        if (x >> 26) in (9, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B) and (x & 0xFFFF) == lo16:
            print(f"{a:08x}: {disassemble(x, a)}")
        if x == t: print(f"{a:08x}: .word {t:#x}")
elif cmd == "words":
    lo, k = int(sys.argv[4], 0), int(sys.argv[5])
    for i in range(k): print(i, hex(w(lo + 4 * i)))
