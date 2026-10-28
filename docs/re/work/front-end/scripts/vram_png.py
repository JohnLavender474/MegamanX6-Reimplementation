import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram
from mmxdemake.png import write_png

def c15(v):
    return bytes([((v >> s) & 31) * 255 // 31 for s in (0, 5, 10)] + [255])

state = read_state(sys.argv[1])
vram = read_vram(state)
x0, y0, w, h = (int(a, 0) for a in sys.argv[3:7]) if len(sys.argv) > 3 else (0, 0, 1024, 512)
rows = []
for y in range(y0, y0 + h):
    vals = struct.unpack_from(f"<{w}H", vram, (y * 1024 + x0) * 2)
    rows.append(b"".join(c15(v) for v in vals))
write_png(sys.argv[2], w, h, rows)
