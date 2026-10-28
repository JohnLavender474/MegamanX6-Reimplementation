import re, sys
from pathlib import Path
import xml.etree.ElementTree as ET

TMX = Path.home() / "mmx-data/x6/tmx/m096-blizzard-wolfang-section-1/blizzard-wolfang-section-1.tmx"
root = ET.parse(TMX).getroot()
W, H = int(root.get("width")), int(root.get("height"))
first = int([t for t in root.findall("tileset") if t.get("name") == "collision"][0].get("firstgid"))
layer = [l for l in root.findall("layer") if l.get("name") == "collision"][0]
cells = [int(v) for v in re.split(r"[,\s]+", layer.find("data").text.strip())]


def col(x, y):
    cx, cy = x // 16, y // 16
    if not (0 <= cx < W and 0 <= cy < H):
        return 0
    g = cells[cy * W + cx]
    return g - first if g else 0


for o in root.find("objectgroup").findall("object"):
    p = {q.get("name"): q.get("value") for q in o.find("properties").findall("property")}
    if p.get("kind") in ("0", "1", "2") and p.get("b") == "6":
        x, y = int(float(o.get("x"))), int(float(o.get("y")))
        col_vals = []
        for dy in range(-16, 96, 16):
            v = col(x, y + dy)
            col_vals.append(f"{y+dy}:{v}")
        print(f"rec {p['index']:>3} kind {p['kind']} id {p['id']} at ({x}, {y})  column:", " ".join(col_vals))
