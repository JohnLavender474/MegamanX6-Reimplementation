import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import banks, scratchpad, clut_word

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()


def u32(r, a):
    return struct.unpack_from('<I', r, a & 0x1FFFFF)[0]


def u16(r, a):
    return struct.unpack_from('<H', r, a & 0x1FFFFF)[0]


for st in sys.argv[1:]:
    p = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
    data = read_state(p)
    r = ram_from(data, EXE, p)
    bl = banks(r, scratchpad(data))
    print("==", st, "globals", [hex(u32(r, 0x800FCFF0 + 4 * k)) for k in range(4)])
    for b in bl:
        if b.type == 30:
            print(f"  bank {b.index}: data {b.data:#x} count {b.count} tex {b.texture:#x} clut {b.clut:#x} "
                  f"(x {(b.clut & 63) * 16}, y {b.clut >> 6})")
    for i in range(48):
        a = 0x8008EF48 + i * 0x9C
        if r[a & 0x1FFFFF] and r[(a & 0x1FFFFF) + 1] == 30:
            print(f"  main[{i}] var {r[(a & 0x1FFFFF) + 2]} x {u32(r, a + 8) >> 16} y {u32(r, a + 12) >> 16} "
                  f"data {u32(r, a + 0x3C):#x} tex {u16(r, a + 0x40):#x} clut {u16(r, a + 0x42):#x} "
                  f"contact {r[(a & 0x1FFFFF) + 0x60]}")
    for i in range(32):
        a = 0x80093C98 + i * 0xA0
        if r[a & 0x1FFFFF] and r[(a & 0x1FFFFF) + 1] == 35:
            print(f"  visual[{i}] type 35 var {r[(a & 0x1FFFFF) + 2]} data {u32(r, a + 0x3C):#x} "
                  f"tex {u16(r, a + 0x40):#x} clut {u16(r, a + 0x42):#x}")
    for addr in range(0x800FC29C, 0x800FC2D0, 4):
        b = r[addr & 0x1FFFFF:(addr & 0x1FFFFF) + 4]
        print(f"   box {addr:#x}: {struct.unpack('<bbBB', b)}")
