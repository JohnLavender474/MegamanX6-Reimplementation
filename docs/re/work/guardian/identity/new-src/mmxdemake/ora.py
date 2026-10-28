import zipfile
from pathlib import Path
from xml.sax.saxutils import quoteattr

from mmxdemake.png import encode_png

MIMETYPE = b"image/openraster"
THUMB = 256


def thumbnail(width, height, rows):
    big = max(width, height)
    if big <= THUMB:
        return width, height, rows
    tw, th = max(1, width * THUMB // big), max(1, height * THUMB // big)
    out = []
    for y in range(th):
        src = rows[y * height // th]
        out.append(b"".join(src[(x * width // tw) * 4:(x * width // tw) * 4 + 4] for x in range(tw)))
    return tw, th, out


def write_ora(path, width, height, layers, merged):
    pngs = [(f"data/layer{n}.png", encode_png(width, height, rows)) for n, (_, rows, _) in enumerate(layers)]
    entries = "".join(f'    <layer name={quoteattr(name)} src="{src}" x="0" y="0" opacity="{opacity:.1f}" '
                      f'visibility="visible"/>\n' for (name, _, opacity), (src, _) in zip(layers, pngs))
    stack = (f'<?xml version="1.0" encoding="UTF-8"?>\n<image version="0.0.3" w="{width}" h="{height}">\n'
             f"  <stack>\n{entries}  </stack>\n</image>\n")
    merged_png = encode_png(width, height, merged)
    thumb_png = encode_png(*thumbnail(width, height, merged))
    with zipfile.ZipFile(Path(path), "w") as z:
        z.writestr(zipfile.ZipInfo("mimetype"), MIMETYPE, compress_type=zipfile.ZIP_STORED)
        z.writestr("stack.xml", stack, compress_type=zipfile.ZIP_DEFLATED)
        for src, data in pngs:
            z.writestr(src, data, compress_type=zipfile.ZIP_STORED)
        z.writestr("mergedimage.png", merged_png, compress_type=zipfile.ZIP_STORED)
        z.writestr("Thumbnails/thumbnail.png", thumb_png, compress_type=zipfile.ZIP_STORED)
