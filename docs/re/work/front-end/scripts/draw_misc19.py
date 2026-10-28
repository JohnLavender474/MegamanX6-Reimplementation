"""Draw every sprite of the misc type 19 source (title and menu text) and the live screen.

Source from the live objects: sprite data +0x3C = 0x8010301C, texture +0x40 = 0x1800 (4 bpp,
VRAM (512, 256)), CLUT +0x42 = 0x7800 (VRAM (0, 480)), mode 0 (flags & 0x60 = 0).
"""
import sys, struct
from pathlib import Path
from PIL import Image as PImage
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from mmxdemake.sprites import Bank, draw, with_source

EXT = Path.home() / "mmx-data/x6/extracted/retail"
EXE = (EXT / "iso/SLUS_013.95").read_bytes()


def to_img(im):
    out = PImage.new("RGBA", (im.width, im.height))
    out.putdata([p for row in im.pixels for p in row])
    return out


def main(state_path, out_dir, data=0x8010301C, texture=0x1800, clut=0x7800):
    st = read_state(state_path)
    ram, vram = ram_from(st, EXE, state_path), read_vram(st)
    bank = with_source(ram, Bank(0, 19, data, 0, texture, clut), data, texture, clut)
    print("sprites", bank.count)
    imgs = []
    for n in range(bank.count):
        for c in (clut, clut + 1):
            im = draw(ram, vram, bank._replace(clut=c), n, mode=0)
            imgs.append((n, c, im))
            print(n, hex(c), im.width, im.height, "origin", im.ox, im.oy)
    cw = max(i.width for _, _, i in imgs) + 8
    ch = max(i.height for _, _, i in imgs) + 8
    sheet = PImage.new("RGBA", (cw * 2, ch * bank.count), (40, 40, 40, 255))
    for k, (n, c, im) in enumerate(imgs):
        sheet.alpha_composite(to_img(im), ((k % 2) * cw + 4, n * ch + 4))
    sheet.save(Path(out_dir) / "misc19-sprites.png")
    return ram, vram, bank


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
