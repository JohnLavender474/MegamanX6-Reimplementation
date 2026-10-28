"""Draw the 512 x 240 screen of a title or menu state: BG layer 0 at camera (0, 0) and the live
misc type 19 objects with display = 1 (or all with --all), in screen space (layer -1)."""
import sys, struct
from pathlib import Path
from PIL import Image as PImage
sys.path.insert(0, str(Path(__file__).parent))
import render_title, draw_misc19
from mmxdemake.sprites import draw

state, out = sys.argv[1], sys.argv[2]
show_all = "--all" in sys.argv
tmp = Path(out).with_suffix(".layer.png")
render_title.main(state, tmp)
bg = PImage.open(tmp).convert("RGBA").crop((0, 0, 512, 240))
tmp.unlink()
ram, vram, bank = draw_misc19.main(state, Path(out).parent)
objs = []
for i in range(96):
    a = (0x800CD410 + i * 0x60) & 0x1FFFFF
    if ram[a] and ram[a + 1] == 19 and (ram[a + 3] or show_all):
        x, y = struct.unpack_from("<h", ram, a + 0xA)[0], struct.unpack_from("<h", ram, a + 0xE)[0]
        clut = struct.unpack_from("<H", ram, a + 0x42)[0]
        objs.append((-ram[a + 0x16], x, y, ram[a + 0x47], clut))
for _, x, y, n, clut in sorted(objs):
    im = draw(ram, vram, bank._replace(clut=clut), n, mode=0)
    pi = PImage.new("RGBA", (im.width, im.height)); pi.putdata([p for r in im.pixels for p in r])
    bg.alpha_composite(pi, (x - im.ox, y - im.oy))
bg.save(out)
print(out, objs)
