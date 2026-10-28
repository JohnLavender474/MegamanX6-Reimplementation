import sys
from PIL import Image
Image.MAX_IMAGE_PIXELS = None
main = Image.open("ora/data/layer2.png").convert("RGBA")
deco = Image.open("ora/data/layer3.png").convert("RGBA")
bg = Image.open("ora/data/layer4.png").convert("RGBA")
out = []
for arg in sys.argv[1:]:
    cx, cy = map(int, arg.split(","))
    v = Image.new("RGBA", (320, 240), (255, 0, 255, 255))
    v.alpha_composite(bg.crop((cx // 2, cy, cx // 2 + 320, cy + 240)))
    v.alpha_composite(main.crop((cx, cy, cx + 320, cy + 240)))
    out.append(v)
sheet = Image.new("RGBA", (330 * len(out), 240 * 2 + 10), (0, 0, 0, 255))
for i, v in enumerate(out):
    sheet.paste(v, (330 * i, 0))
    cx, cy = map(int, sys.argv[1 + i].split(","))
    sheet.paste(main.crop((cx, cy, cx + 320, cy + 240)), (330 * i, 250))
sheet.save("compose.png")
