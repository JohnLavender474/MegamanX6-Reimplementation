"""Print the layer-0 collision values below each grey block of the first tower (from the TMX), to find the
fall height. Usage: python3 block_floors.py"""
import re, sys
from pathlib import Path
tmx = (Path.home() / "mmx-data/x6/tmx/m097-blaze-heatnix-section-1/blaze-heatnix-section-1.tmx").read_text()
m = re.search(r'<layer[^>]*name="collision"[^>]*width="(\d+)"[^>]*height="(\d+)".*?<data encoding="csv">(.*?)</data>', tmx, re.S)
wd, ht = int(m.group(1)), int(m.group(2))
cells = [int(v) for v in m.group(3).replace("\n", "").split(",")]
gids = [(int(g), n) for g, n in re.findall(r'<tileset firstgid="(\d+)"[^>]*name="([^"]+)"', tmx)]
first = [g for g, n in gids if "collision" in n][0]
val = lambda cx, cy: (cells[cy * wd + cx] - first) if cells[cy * wd + cx] else -1
for rec, x, y in ((111, 1056, 712), (112, 1152, 872), (113, 1184, 1240), (114, 1152, 1528), (115, 992, 2104)):
    print(f"record {rec} at ({x}, {y}); columns x {x-32}-{x+31}")
    for ty in range((y + 24) // 16, min(ht, (y + 24) // 16 + 24)):
        row = [val(cx, ty) for cx in range((x - 32) // 16, (x + 32) // 16)]
        print(f"  y {ty*16:5d}: {row}")
        if all(v > 0 for v in row):
            break
