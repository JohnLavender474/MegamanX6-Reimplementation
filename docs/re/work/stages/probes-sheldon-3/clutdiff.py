import sys
from pathlib import Path
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW
v = [read_vram(read_state(Path.home() / 'mmx-data/x6/dumps' / s)) for s in sys.argv[1:]]
for y in range(448, 512):
    for x0 in range(0, 1024, 16):
        o = y * ROW + x0 * 2
        segs = [d[o:o + 32] for d in v]
        if len(set(segs)) > 1:
            print(y, x0, [(i, segs[0][2*i:2*i+2].hex(), segs[1][2*i:2*i+2].hex()) for i in range(16) if segs[0][2*i:2*i+2] != segs[1][2*i:2*i+2]])
