import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
D = Path.home()/"mmx-data/x6/dumps"
mem = {m: (X/f"rock_dat/{m:03d}.bin").read_bytes() for m in range(85, 90)}
u32 = lambda r, a: struct.unpack_from("<I", r, a)[0]
for n in sys.argv[1:]:
    p = D/n; data = read_state(p); r = ram_from(data, exe, p); s = scratchpad(data)
    fd, sd = struct.unpack_from("<I", s, 0x14)[0], struct.unpack_from("<I", s, 0x1C)[0]
    print(n, "g38", r[0xCCED0+0x38], "framedir", hex(fd), "spritedir", hex(sd))
    print("  frame dir", [hex(u32(r, (fd & 0x1FFFFF) + 4*k)) for k in range(12)])
    print("  sprite dir", [hex(u32(r, (sd & 0x1FFFFF) + 4*k)) for k in range(20)])
    for lo, hi, what in ((fd, fd + 0x40, "framedir"), (sd, sd + 0x40, "spritedir"), (0x80103acc, 0x80103acc+0x1000, "frames0"), (u32(r, 0x970A0+0x3C), u32(r, 0x970A0+0x3C)+0x200, "sprites mega+3C")):
        blob = r[lo & 0x1FFFFF: hi & 0x1FFFFF]
        hits = [(m, b.find(blob)) for m, b in mem.items() if b.find(blob) >= 0]
        print("  ", what, hex(lo), "in members", [(m, hex(o), hex(lo - o)) for m, o in hits])
