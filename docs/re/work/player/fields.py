import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x970A0
u16 = lambda r, a: struct.unpack_from("<H", r, a)[0]
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
for s in sys.argv[1:]:
    p = Path.home()/"mmx-data/x6/dumps"/s
    r = ram_from(read_state(p), exe, p)
    d = u32(r, M+0x3C) - 0x80000000
    print(f"{s}: f0 {r[M]:#x} var {r[M+2]} step {r[M+5]} +30 {u32(r,M+0x30):#x} +34 {u32(r,M+0x34):#x} +44 {u16(r,M+0x44)} +46..4B {r[M+0x46:M+0x4C].hex(' ')} "
          f"sprites: {u16(r,d+2)} (+0 {u16(r,d)}) first {[ (u16(r,d+4*i), u16(r,d+4*i+2)) for i in range(4)]}")
