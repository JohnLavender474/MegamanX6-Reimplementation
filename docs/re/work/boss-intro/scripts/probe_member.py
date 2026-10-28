import sys
from collections import Counter
from common import *
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, flags
from mmxdemake.texture import tile_ref, bits
from mmxdemake.palettes import sprites, cameras, learn_rule, layers_from, PaletteError
m = int(sys.argv[1]) if len(sys.argv) > 1 else 142
ram, vram = load()
b = (EXT / f'rock_dat/{m:03d}.bin').read_bytes()
es = split(b)
screens = decode_screens(entry_data(b, only(es, 0)))
info = decode_tile_info(entry_data(b, only(es, 1)))
print('screens', len(screens), 'tiles', len(info))
pages = Counter(); pals = Counter(); fl = Counter()
for s in screens:
    for v in s:
        t = tile(v); fl[flags(v)] += 1
        if t: r = tile_ref(info[t]); pages[r.page] += 1; pals[(r.page, r.palette)] += 1
print('pages', pages); print('pal', pals); print('flags', fl)
w, h = 24, 8
layers = layers_from(ram[0x97438:0x97438 + 3 * w * h], w, h)
sp = sprites(ram)
print('sprite cmds', len(sp))
try:
    cams = cameras(sp, layers, w, h, screens, info); print('cams', cams)
    print(learn_rule(sp, cams, layers, w, h, screens, info))
except PaletteError as e:
    print('palette error', e)
