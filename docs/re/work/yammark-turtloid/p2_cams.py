import sys
from probe_ram import *
for st in sys.argv[1:]:
    ram, scr = load(st)
    print(st, "stage", u8(ram,0x800CCEDC), "area", u8(ram,0x800CCEDD), "mega x,y", s16(ram,0x800970A0+0x0A) if False else hex(u32(ram,0x800970A0+0x08)), hex(u32(ram,0x800970A0+0x0C)))
    for L in range(3):
        b = 0x800971F8 + L*0x54
        print(f"  L{L} mode {u8(ram,b+4)} x {s16(ram,b+0x0A)} y {s16(ram,b+0x0E)} +40 {s16(ram,b+0x40)} +42 {s16(ram,b+0x42)} lim {[s16(ram,b+0x1C+2*k) for k in range(4)]}  raw {hexd(ram,b,0x54)}")
