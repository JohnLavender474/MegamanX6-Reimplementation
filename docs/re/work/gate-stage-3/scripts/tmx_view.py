import sys
import xml.etree.ElementTree as ET
from pathlib import Path

from PIL import Image

tmx = Path(sys.argv[1])
out = sys.argv[2]
x0, y0, x1, y1 = (int(v) for v in sys.argv[3:7]) if len(sys.argv) > 6 else (0, 0, None, None)
layers = sys.argv[7].split(",") if len(sys.argv) > 7 else ["art 2 background", "art 1 decoration", "art 0 main", "art 0 front"]
root = ET.parse(tmx).getroot()
w, h = int(root.get("width")), int(root.get("height"))
ts = root.find("tileset")
first = int(ts.get("firstgid"))
cols = int(ts.get("columns"))
sheet = Image.open(tmx.parent / ts.find("image").get("source")).convert("RGBA")
x1 = x1 or w * 16
y1 = y1 or h * 16
img = Image.new("RGBA", (x1 - x0, y1 - y0), (0, 0, 0, 255))
for layer in root.iter("layer"):
    if layer.get("name") not in layers:
        continue
    cells = [int(v) for v in layer.find("data").text.replace("\n", "").split(",")]
    for cy in range(y0 // 16, (y1 + 15) // 16):
        for cx in range(x0 // 16, (x1 + 15) // 16):
            gid = cells[cy * w + cx] & 0x1FFFFFFF
            if not gid or gid - first >= int(ts.get("tilecount")):
                continue
            t = gid - first
            tile = sheet.crop(((t % cols) * 16, (t // cols) * 16, (t % cols) * 16 + 16, (t // cols) * 16 + 16))
            img.alpha_composite(tile, (cx * 16 - x0, cy * 16 - y0))
scale = float(sys.argv[8]) if len(sys.argv) > 8 else 0.5
img.resize((int(img.width * scale), int(img.height * scale))).save(out)
