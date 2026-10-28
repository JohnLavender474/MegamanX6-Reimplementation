"""Show a VRAM area as 4 bpp texels with one CLUT: vram4.py state out x0 y0 w16 h clut_x clut_y."""
import sys, struct
from pathlib import Path
from PIL import Image
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram

v = read_vram(read_state(sys.argv[1]))
x0, y0, w16, h, cx, cy = (int(a, 0) for a in sys.argv[3:9])
pal = struct.unpack_from("<16H", v, cy * 2048 + cx * 2)
col = [(((c & 31) * 255 // 31), ((c >> 5 & 31) * 255 // 31), ((c >> 10 & 31) * 255 // 31)) for c in pal]
img = Image.new("RGB", (w16 * 4, h))
px = img.load()
for y in range(h):
    for x in range(w16):
        hw = struct.unpack_from("<H", v, (y0 + y) * 2048 + (x0 + x) * 2)[0]
        for k in range(4):
            px[x * 4 + k, y] = col[(hw >> (4 * k)) & 15]
img.save(sys.argv[2])
