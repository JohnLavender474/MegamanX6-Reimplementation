import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x970A0
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
for s, mem in (("intro-1_start.state.gz", 87), ("turtloid-6_zero-player.state.gz", 85)):
    p = Path.home()/"mmx-data/x6/dumps"/s
    data = read_state(p); r = ram_from(data, exe, p); v = read_vram(data)
    b = (X/f"rock_dat/{mem:03d}.bin").read_bytes()
    print(s, mem)
    for name, a, n in (("anim table", u32(r, M+0x30), 64), ("anim first list", u32(r, u32(r, M+0x30) - 0x80000000), 32),
                       ("sprite data", u32(r, M+0x3C), 64), ("frame table", u32(r, M+0x38), 64)):
        o = a - 0x80000000; chunk = r[o:o+n]
        hits = [i for i in range(len(b)) if b.startswith(chunk, i)][:3]
        print(f"  {name} {a:#x}: member offsets {[hex(h) for h in hits]}  delta {[hex(a - h) for h in hits]}")
    pal = v[480*2048:480*2048+32*16]
    print("  clut row 480 first 32 bytes", pal[:32].hex(), "in member at", [hex(i) for i in range(len(b)) if b.startswith(pal[:32], i)][:4])
    print("  header", [hex(w) for w in struct.unpack_from("<24I", b, 0)])
