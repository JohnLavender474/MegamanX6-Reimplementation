import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram
from mmxdemake.png import write_png

vram = read_vram(read_state(sys.argv[1]))
y0 = int(sys.argv[3], 0)
rows = []
for y in range(y0, y0 + 240):
    b = vram[y * 2048:y * 2048 + 960]
    rows.append(b"".join(b[i:i + 3] + b"\xff" for i in range(0, 960, 3)))
write_png(sys.argv[2], 320, 240, rows)
