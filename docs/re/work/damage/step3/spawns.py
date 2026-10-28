import sys
sys.path.insert(0, sys.argv[0].rsplit("/", 1)[0] or ".")
import ram
from mmxdemake.mips import disassemble
r = ram.load(sys.argv[1])
lo, hi = int(sys.argv[2], 0), int(sys.argv[3], 0)
targets = {0x8003D3F8: "SpawnWeapon", 0x8003D480: "SpawnWeaponFrom"}
for a in range(lo, hi, 4):
    w = ram.word(r, a)
    if w >> 26 == 3:
        t = 0x80000000 | ((w & 0x3FFFFFF) << 2)
        if t in targets:
            a1 = None
            for b in list(range(a - 40, a, 4)) + [a + 4]:
                x = ram.word(r, b)
                if x >> 26 == 9 and (x >> 16) & 31 == 5 and (x >> 21) & 31 == 0:
                    a1 = x & 0xFFFF
                    at = b
            print(f"{a:08x} {targets[t]} a1={a1} " + (f"@{at:08x}" if a1 is not None else ""))
