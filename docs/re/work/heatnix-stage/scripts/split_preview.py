"""Preview a texture_split: take the art layers of two stage.ora drawings (block 0x10000 and
block 0x16), use the second inside the regions [layer, x0, y0, x1, y1], and composite
layers 2, 1, 0 on black. Usage: python3 split_preview.py <a.ora> <b.ora> <out.png> <scale> <regions json>"""
import io, json, sys, zipfile
from PIL import Image

a, b, out, scale, regions = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), json.loads(sys.argv[5])
ART = {0: 2, 1: 3, 2: 4}


def layer(path, n):
    return Image.open(io.BytesIO(zipfile.ZipFile(path).read(f"data/layer{ART[n]}.png"))).convert("RGBA")


canvas = None
for n in (2, 1, 0):
    la, lb = layer(a, n), layer(b, n)
    for l, x0, y0, x1, y1 in regions:
        if l == n:
            la.paste(lb.crop((x0, y0, x1, y1)), (x0, y0))
    canvas = Image.new("RGBA", la.size, (0, 0, 0, 255)) if canvas is None else canvas
    canvas = Image.alpha_composite(canvas, la)
canvas.resize((canvas.size[0] // scale, canvas.size[1] // scale)).save(out)
