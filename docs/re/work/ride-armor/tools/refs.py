import sys
sys.path.insert(0, str(__import__("pathlib").Path(__file__).parent))
from ramtool import ram, w32, dis
r = ram(sys.argv[1]); target = int(sys.argv[2], 16)
hi = (target + 0x8000) >> 16; lo = target & 0xFFFF
for a in range(0x80010000, 0x80200000, 4):
    w = w32(r, a)
    if (w >> 26) == 0xF and (w & 0xFFFF) == hi:
        rt = (w >> 16) & 31
        for k in range(1, 12):
            w2 = w32(r, a + 4*k)
            if ((w2 >> 21) & 31) == rt and (w2 & 0xFFFF) == lo and (w2 >> 26) not in (0xF,):
                print(dis(r, a + 4*k, 1)); break
