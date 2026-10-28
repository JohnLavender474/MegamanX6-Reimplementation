import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
M = 0x800970A0
u32 = lambda r, a: struct.unpack_from("<I", r, a - 0x80000000)[0]
files = {"exe": exe} | {f"dat/{p.stem}": p.read_bytes() for p in sorted((X/"rock_dat").glob("*.bin"))} | {f"bin/{p.stem}": p.read_bytes() for p in sorted((X/"rock_bin").glob("*.bin"))}
for s in ("intro-1_start.state.gz", "turtloid-6_zero-player.state.gz"):
    p = Path.home()/"mmx-data/x6/dumps"/s
    r = ram_from(read_state(p), exe, p)
    t = u32(r, M+0x30)
    ptrs = []
    while True:
        w = u32(r, t + 4*len(ptrs))
        if not (0x801E0000 <= w < t): break
        ptrs.append(w)
    lo = min(ptrs)
    print(f"{s}: table {t:#x}, {len(ptrs)} animations, lists {lo:#x}..{t:#x} ({t-lo} bytes); after table: {[hex(u32(r, t+4*len(ptrs)+4*i)) for i in range(4)]}")
    print("  first list words", [hex(u32(r, ptrs[0]+4*i)) for i in range(6)])
    chunk = r[lo-0x80000000:lo-0x80000000+16]
    for name, b in files.items():
        i = b.find(chunk)
        if i >= 0: print("  found 16 bytes in", name, hex(i))
    rel = struct.pack("<4I", *[w - lo for w in ptrs[:4]])
    rel2 = struct.pack("<4I", *[w - t for w in ptrs[:4]])
    for name, b in files.items():
        for lab, c in (("rel-lists", rel), ("rel-table", rel2)):
            i = b.find(c)
            if i >= 0: print("  table as", lab, "offsets in", name, hex(i))
    print("  region before lists:", [hex(u32(r, lo - 16 + 4*i)) for i in range(4)], " +0x34", hex(u32(r, M+0x34)))
