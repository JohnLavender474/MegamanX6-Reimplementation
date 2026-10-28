import sys, os, struct
from pathlib import Path
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, collision
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
b = (Path.home() / 'mmx-data/x6/extracted/retail/rock_dat/102.bin').read_bytes()
disc = decode_screens(entry_data(b, only(split(b), 0)))
info = decode_tile_info(entry_data(b, only(split(b), 1)))
W, H = 24, 8
def load(st):
    p = os.path.expanduser('~/mmx-data/x6/dumps/' + st)
    d = read_state(p); r = ram_from(d, EXE, p); s = scratchpad(d)
    lay = struct.unpack_from('<I', s, 4)[0] & 0x1fffff
    scr = struct.unpack_from('<I', s, 8)[0] & 0x1fffff
    live = decode_screens(r[scr:scr + len(disc) * 0x200])
    return r, r[lay:lay + W * H], live, hex(0x80000000 + scr)
def cell(layout, screens, x, y):
    sc = layout[(y // 256) * W + x // 256]
    v = screens[sc][((y % 256) // 16) * 16 + (x % 256) // 16]
    return sc, tile(v), collision(info[tile(v)])
if __name__ == '__main__':
    for st in sys.argv[1:]:
        r, layout, live, base = load(st)
        diffs = []
        for s in range(len(disc)):
            for n in range(256):
                if disc[s][n] != live[s][n]:
                    places = [(i % W * 256 + (n % 16) * 16, i // W * 256 + (n // 16) * 16) for i, v in enumerate(layout) if v == s]
                    diffs.append((s, n % 16, n // 16, collision(info[tile(disc[s][n])]), collision(info[tile(live[s][n])]), places))
        print('==', st, 'screens at', base, len(diffs), 'cells differ')
        for d in diffs:
            print('  screen', d[0], 'cell', d[1], d[2], 'coll', d[3], '->', d[4], 'at', d[5])
