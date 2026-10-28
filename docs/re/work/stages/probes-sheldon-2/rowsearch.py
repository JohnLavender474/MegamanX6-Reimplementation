import sys
from pathlib import Path
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW

ex = Path.home() / "mmx-data/x6/extracted/retail"
v = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / sys.argv[1]))
probes = {}
for p in (2, 3, 4, 5, 6):
    for r in (37, 101, 170):
        x = 320 + p * 64
        o = (256 + r) * ROW + x * 2
        piece = v[o:o + 64]
        if len(set(piece)) > 8:
            probes[(p, r)] = piece
files = [f for f in ex.rglob("*") if f.is_file()]
for f in files:
    d = f.read_bytes()
    for k, piece in probes.items():
        i = d.find(piece)
        if i >= 0:
            print(k, f.relative_to(ex), hex(i))
print("probes", len(probes), "files", len(files))
