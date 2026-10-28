import sys, json
from pathlib import Path
from PIL import Image, ImageDraw
d = Path(sys.argv[1]); out = sys.argv[2]
meta = json.load(open(d/"animations.json"))
anims = sorted(d.glob("anim-*.png"), key=lambda p: int(p.stem.split("-")[1]))
ims = [(p.stem, Image.open(p).convert("RGBA")) for p in anims]
W = max(i.width for _, i in ims) + 60; H = sum(i.height + 4 for _, i in ims)
sheet = Image.new("RGBA", (min(W, 4000), H), (40, 40, 60, 255)); dr = ImageDraw.Draw(sheet); y = 0
for n, i in ims:
    dr.text((2, y + 2), n.split("-")[1], fill=(255, 255, 0, 255)); sheet.paste(i, (60, y), i); y += i.height + 4
sheet.save(out); print(sheet.size, meta.get("cell"), meta.get("origin"))
