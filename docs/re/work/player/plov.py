import sys
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
X = Path.home()/"mmx-data/x6/extracted/retail"
exe = (X/"iso/SLUS_013.95").read_bytes()
ov = {int(p.stem): p.read_bytes() for p in sorted((X/"rock_bin").glob("*.bin"))}
print("sizes", {n: len(b) for n, b in ov.items() if n < 8})
for s in ("intro-1_start.state.gz", "yammark-1_start.state.gz", "turtloid-6_zero-player.state.gz", "turtloid-8_high-max.state.gz", "report-1.state.gz"):
    p = Path.home()/"mmx-data/x6/dumps"/s
    r = ram_from(read_state(p), exe, p)
    base = 0x1EA000
    m = [(n, sum(1 for i in range(len(b)) if r[base+i] != b[i])) for n, b in ov.items() if base + len(b) <= len(r) and r[base:base+256] == b[:256]]
    print(s, "overlays at 0x801EA000:", m)
