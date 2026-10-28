import sys, json
from collections import Counter
from pathlib import Path
from probe_ram import *
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, SIDE
from mmxdemake.texture import tile_ref
member = int(sys.argv[1]); stagedir = sys.argv[2]; pals = [int(x) for x in sys.argv[3].split(",")]
body = (Path.home() / f"mmx-data/x6/extracted/retail/rock_dat/{member:03d}.bin").read_bytes()
ents = split(body)
screens = decode_screens(entry_data(body, only(ents, 0)))
info = decode_tile_info(entry_data(body, only(ents, 1)))
layers = json.load(open(Path.home() / f"mmx-data/x6/stages/{stagedir}/layout.json"))["layers"]
for p in pals:
    c = Counter()
    for L, lay in enumerate(layers):
        for sy, row in enumerate(lay):
            for sx, s in enumerate(row):
                if not s or s >= len(screens): continue
                for k, v in enumerate(screens[s]):
                    t = tile(v)
                    if t and t < len(info) and tile_ref(info[t]).palette == p:
                        c[(L, sx, sy)] += 1
    print("tile palette", p, "cells by (layer, screen x, y):", sorted(c.items())[:60], "total", sum(c.values()))
from PIL import Image, ImageDraw
Image.MAX_IMAGE_PIXELS = None
img = Image.open(Path.home() / f"mmx-data/x6/stages/{stagedir}/stage.png").convert("RGB")
dr = ImageDraw.Draw(img)
cols = [(255,0,0),(0,255,255),(255,255,0)]
for i, p in enumerate(pals):
    lay = layers[0]
    for sy, row in enumerate(lay):
        for sx, s in enumerate(row):
            if not s or s >= len(screens): continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                if t and t < len(info) and tile_ref(info[t]).palette == p:
                    x = sx*256 + (k % SIDE)*16; y = sy*256 + (k // SIDE)*16
                    dr.rectangle([x, y, x+15, y+15], outline=cols[i])
crops = [img.crop(tuple(map(int, c.split(",")))) for c in sys.argv[4:]]
W = sum(c.width for c in crops) + 10*len(crops); H = max(c.height for c in crops)
sheet = Image.new("RGB", (W, H))
x = 0
for c in crops:
    sheet.paste(c, (x, 0)); x += c.width + 10
sheet.save("paltiles.png")
