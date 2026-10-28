import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M, G, W, TT = 0x970A0, 0xCCED0, 0x950A0, 0x71220
u8 = lambda r, a: r[a]
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
names = sys.argv[1:]
for p in names:
    p = Path(p); data = read_state(p); r = ram_from(data, exe, p)
    print(p.name, "stage", r[G+0xC], "area", r[G+0xD], "char/form +5E", r[G+0x5E], "mega.form", r[M+0xE7],
          "armors +5F", hex(r[G+0x5F]), "parts +69", hex(r[G+0x69]), "mega+EC", hex(r[M+0xEC]),
          "step", r[M+0x02:M+0x06].hex(), "anim +47?", r[M+0x47])
    for k in range(32):
        o = W + k*0xA0
        if r[o]:
            print(f"   weapon {k:2d} @ {0x80000000+o:#x}: head {r[o:o+8].hex()} type {r[o+1]} x {struct.unpack_from('<h', r, o+0x18)[0] if False else ''}")
