"""Draw the boss intro picture of each Maverick (game mode 6, overlay 17).

Boss b (1-8, game+0x36): screens, tile info and texture from ROCK_X6.DAT member 140 + b,
palettes from member 51 + b (type-5 entry, row n = VRAM row 480 + n; BG CLUT base (0, 486)).
Layout (stage 13 area 1, 24 x 8, only row 0 used): layer 0 screens 1, 2 at x 2, 3 (the name),
layer 1 screens 3, 4 at x 2, 3 (the full body), layer 2 screens 5, 6, 7 at x 0-2 (the close-up).
Final camera x: layer 0 = overlay 17 table 0x800EA518[b-1], layer 1 = 0x800EA508[b-1], layer 2 = 0.
View: 512 x 240 (SetDisplayMode(1) in game mode 5).
"""
import sys, struct
from pathlib import Path
import numpy as np
from PIL import Image
from common import EXT
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, SIDE
from mmxdemake.texture import tile_ref, raw_tile, bits

LAYOUT = [[0, 0, 1, 2], [0, 0, 3, 4], [5, 6, 7, 0]]
PAL_ROW0 = 480
CLUT_Y = 486
OV17_BASE = 0x800E9860
VIEW_W, VIEW_H = 512, 240


def rgba(c):
    r, g, b = (c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3
    return (r | r >> 5, g | g >> 5, b | b >> 5, 0 if c == 0 else 255)


def palettes(member):
    d = (EXT / f'rock_dat/{member:03d}.bin').read_bytes()
    body = entry_data(d, only(split(d), 5))
    return {PAL_ROW0 + n: [rgba(c) for c in struct.unpack_from('<256H', body, n * 512)]
            for n in range(len(body) // 512)}


def member_data(member):
    b = (EXT / f'rock_dat/{member:03d}.bin').read_bytes()
    es = split(b)
    return (decode_screens(entry_data(b, only(es, 0))), decode_tile_info(entry_data(b, only(es, 1))),
            entry_data(b, only(es, 0x10014)))


def targets(boss):
    ov = (EXT / 'rock_bin/17.bin').read_bytes()
    l1 = struct.unpack_from('<h', ov, 0x800EA508 - OV17_BASE + 2 * (boss - 1))[0]
    l0 = struct.unpack_from('<h', ov, 0x800EA518 - OV17_BASE + 2 * (boss - 1))[0]
    return {0: l0, 1: l1, 2: 0}


def draw_screen(screen, info, block, pals):
    img = np.zeros((256, 256, 4), np.uint8)
    for k, v in enumerate(screen):
        t = tile(v)
        if not t:
            continue
        ref = tile_ref(info[t])
        vals = raw_tile(block, ref)
        if vals is None:
            continue
        row = pals[CLUT_Y + ref.palette // 16]
        x0 = (ref.palette % 16) * 16
        cols = [row[x0 + v] if bits(ref.page) == 4 else row[v] for v in vals]
        px, py = (k % SIDE) * 16, (k // SIDE) * 16
        img[py:py + 16, px:px + 16] = np.array(cols, np.uint8).reshape(16, 16, 4)
    return img


def layer_image(layer, screens, info, block, pals):
    img = np.zeros((256, 1024, 4), np.uint8)
    for x, s in enumerate(LAYOUT[layer]):
        if s:
            img[:, x * 256:(x + 1) * 256] = draw_screen(screens[s], info, block, pals)
    return img


def crop_used(img):
    ys, xs = np.nonzero(img[..., 3])
    return img[ys.min():ys.max() + 1, xs.min():xs.max() + 1]


def run(boss, out):
    out = Path(out); out.mkdir(parents=True, exist_ok=True)
    member, pal_member = 140 + boss, 51 + boss
    pals = palettes(pal_member)
    screens, info, block = member_data(member)
    layers = [layer_image(l, screens, info, block, pals) for l in range(3)]
    names = {0: 'name', 1: 'body', 2: 'closeup'}
    for l, im in enumerate(layers):
        Image.fromarray(crop_used(im)).save(out / f'boss{boss}-m{member}-layer{l}-{names[l]}.png')
    tg = targets(boss)
    view = np.zeros((VIEW_H, VIEW_W, 4), np.uint8); view[..., 3] = 255
    for l in (2, 1, 0):
        x = tg[l]
        part = np.zeros((VIEW_H, VIEW_W, 4), np.uint8)
        src = layers[l][:VIEW_H, max(x, 0):x + VIEW_W]
        part[:, :src.shape[1]] = src
        view = np.where(part[..., 3:4] > 0, part, view)
    Image.fromarray(view).save(out / f'boss{boss}-m{member}-final-view.png')
    return member, pal_member, tg


if __name__ == '__main__':
    out = sys.argv[1]
    for b in (map(int, sys.argv[2:]) if len(sys.argv) > 2 else range(1, 9)):
        print(b, run(b, out))
