import sys
from pathlib import Path
from mmxdemake.arcentries import entry_data, split
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW

ex = Path.home() / "mmx-data/x6/extracted/retail"
body = (ex / "rock_dat/102.bin").read_bytes()
blk = entry_data(body, next(e for e in split(body) if e.type == 0x16))
first = entry_data(body, next(e for e in split(body) if e.type == 0x10000))
for st in sys.argv[1:]:
    v = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / st))
    a, b, rows = [], [], []
    for p in range(8):
        bad = badf = 0
        for r in range(256):
            o = (256 + r) * ROW + (320 + p * 64) * 2
            if v[o:o + 128] != blk[r * 1024 + p * 128:r * 1024 + p * 128 + 128]:
                bad += 1
            if v[o:o + 128] != first[p * 0x8000 + r * 128:p * 0x8000 + r * 128 + 128]:
                badf += 1
        a.append(bad); b.append(badf)
    print(st, "0x16 row-major", a, "0x10000 page-major", b)
