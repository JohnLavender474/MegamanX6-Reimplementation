"""Draw stage 14 area 0 (member 107; the title and main-menu picture) from a save state.

Layout from RAM (0x80097438, 4 x 2; layer 0 row 0 = screens 1-4, layers 1 and 2 empty).
Screens (entry 0), tile info (entry 1), texture (entry 0x10014, VRAM (512, 0)) from member 107.
CLUT rows from VRAM (BG CLUT base y 486, as in the boss intro). View 512 x 240 at camera (0, 0).
"""
import sys, struct
from pathlib import Path
import numpy as np
from PIL import Image
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.vram import read_vram
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, SIDE
from mmxdemake.texture import tile_ref, raw_tile, bits

EXT = Path.home() / "mmx-data/x6/extracted/retail"
EXE = (EXT / "iso/SLUS_013.95").read_bytes()
CLUT_Y = 486


def rgba(c):
    r, g, b = (c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3
    return (r | r >> 5, g | g >> 5, b | b >> 5, 0 if c == 0 else 255)


def main(state_path, out):
    st = read_state(state_path)
    ram, vram = ram_from(st, EXE, state_path), read_vram(st)
    w, h = ram[0x800CD338 & 0x1FFFFF], ram[0x800CD339 & 0x1FFFFF]
    lay = ram[0x97438:0x97438 + 3 * w * h]
    d = (EXT / "rock_dat/107.bin").read_bytes(); es = split(d)
    screens = decode_screens(entry_data(d, only(es, 0)))
    info = decode_tile_info(entry_data(d, only(es, 1)))
    block = entry_data(d, only(es, 0x10014))
    pals = {y: [rgba(c) for c in struct.unpack_from("<256H", vram, y * 2048)] for y in range(480, 512)}
    img = np.zeros((h * 256, w * 256, 4), np.uint8)
    used = set()
    for sy in range(h):
        for sx in range(w):
            s = lay[sy * w + sx]
            if not s:
                continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                if not t:
                    continue
                ref = tile_ref(info[t])
                vals = raw_tile(block, ref)
                if vals is None:
                    continue
                used.add((ref.palette, bits(ref.page)))
                row = pals[CLUT_Y + ref.palette // 16]
                x0 = (ref.palette % 16) * 16
                cols = [row[x0 + q] if bits(ref.page) == 4 else row[q] for q in vals]
                px, py = sx * 256 + (k % SIDE) * 16, sy * 256 + (k // SIDE) * 16
                img[py:py + 16, px:px + 16] = np.array(cols, np.uint8).reshape(16, 16, 4)
    Image.fromarray(img).save(out)
    print("screens", len(screens), "palettes/bpp used", sorted(used))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
