import sys, json
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools"); sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools/tests")
from pathlib import Path
from pngread import read_png
from mmxdemake.png import write_png
from mmxdemake.digits import text_mask
D = Path.home()/"mmx-data/x6/sprites/players"/sys.argv[1]
doc = json.load(open(D/"animations.json")); cw, ch = doc["cell"]
anims = [a for a in doc["animations"] if int(sys.argv[3]) <= a["index"] < int(sys.argv[4])]
cols = 6; W = cols * (2 * cw + 6); rows = -(-len(anims) // cols); H = rows * (ch + 10)
canvas = [bytearray(bytes((40, 40, 60, 255)) * W) for _ in range(H)]
for i, a in enumerate(anims):
    w, h, px = read_png(D / f"anim-{a['index']}.png"); n = len(a["frames"])
    x0, y0 = (i % cols) * (2 * cw + 6), (i // cols) * (ch + 10)
    for j, k in enumerate((0, n // 2)):
        for y in range(h):
            row = px[y][k * cw * 4:(k + 1) * cw * 4]
            for x in range(cw):
                p = row[x*4:x*4+4]
                if p[3]: canvas[y0 + 9 + y][(x0 + j * cw + x) * 4:(x0 + j * cw + x) * 4 + 4] = p
    for y, line in enumerate(text_mask(f"{a['index']}")):
        for x, b in enumerate(line):
            if b: canvas[y0 + y][(x0 + x) * 4:(x0 + x) * 4 + 4] = b"\xff\xff\xff\xff"
write_png(Path(sys.argv[2]), W, H, [bytes(r) for r in canvas])
print(len(anims), W, H)
