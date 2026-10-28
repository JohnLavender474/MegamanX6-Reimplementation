import re
import sys
from pathlib import Path

from PIL import Image, ImageDraw

folder = Path(sys.argv[1])
out = sys.argv[2]
maxw = int(sys.argv[3]) if len(sys.argv) > 3 else 1600
files = sorted(folder.glob("anim-*.png"), key=lambda p: [int(n) if n.isdigit() else n for n in re.split(r"(\d+)", p.name)])
files += sorted(folder.glob("sprites*.png"))
ims = [(p.stem, Image.open(p).convert("RGBA")) for p in files]
rows = []
for name, im in ims:
    if im.width > maxw:
        im = im.crop((0, 0, maxw, im.height))
    rows.append((name, im))
h = sum(im.height + 14 for _, im in rows)
w = max([im.width for _, im in rows] + [200])
sheet = Image.new("RGBA", (w, h), (60, 60, 90, 255))
d = ImageDraw.Draw(sheet)
y = 0
for name, im in rows:
    d.text((2, y), name, fill=(255, 255, 0, 255))
    sheet.alpha_composite(im, (0, y + 12))
    y += im.height + 14
sheet.save(out)
print(sheet.size)
