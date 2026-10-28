import sys
from PIL import Image, ImageDraw
src = sys.argv[1]; out = sys.argv[2]; anims = [int(a) for a in sys.argv[3].split(',')]
maxf = int(sys.argv[4]) if len(sys.argv) > 4 else 8
W, H = 177, 179
rows = []
for a in anims:
    im = Image.open(f"{src}/anim-{a}.png").convert("RGBA")
    n = im.width // W
    rows.append((a, [im.crop((i*W, 0, (i+1)*W, H)) for i in range(min(n, maxf))]))
cols = max(len(r[1]) for r in rows)
sheet = Image.new("RGBA", (cols*W + 30, len(rows)*H), (40, 40, 60, 255))
d = ImageDraw.Draw(sheet)
for y, (a, fr) in enumerate(rows):
    d.text((2, y*H + 2), str(a), fill=(255, 255, 0, 255))
    for x, f in enumerate(fr):
        sheet.alpha_composite(f, (30 + x*W, y*H))
        d.rectangle((30 + x*W, y*H, 30 + x*W + W - 1, y*H + H - 1), outline=(80, 80, 100, 255))
        d.line((30 + x*W + 65 - 3, y*H + 91, 30 + x*W + 65 + 3, y*H + 91), fill=(255, 0, 0, 255))
        d.line((30 + x*W + 65, y*H + 88, 30 + x*W + 65, y*H + 94), fill=(255, 0, 0, 255))
sheet.save(out)
