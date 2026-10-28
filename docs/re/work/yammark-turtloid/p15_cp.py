import sys
from probe_ram import *
st, stage, area, n = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])
ram, scr = load(st)
lst = u32(ram, 0x80073300 + (stage*2+area)*4)
print(st, "stage", stage, "area", area, "list", hex(lst))
for c in range(n):
    r = u32(ram, lst + 4*c)
    if not (0x80000000 <= r < 0x80200000): print(" cp", c, "bad", hex(r)); break
    f = [s16(ram, r+2*k) for k in range(12)]
    print(f" cp {c} @{r:#x}: player ({f[0]}, {f[1]}) cam0 ({f[2]}, {f[3]}) cam1 ({f[4]}, {f[5]}) cam2 ({f[6]}, {f[7]}) limits max x {f[8]} min x {f[9]} max y {f[10]} min y {f[11]} facing {u8(ram,r+24):#x} l1 off ({s16(ram,r+26)}, {s16(ram,r+28)}) l2 off ({s16(ram,r+30)}, {s16(ram,r+32)}) b34 {u8(ram,r+34)}")
