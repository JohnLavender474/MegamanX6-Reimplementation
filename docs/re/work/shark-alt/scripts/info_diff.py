import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.index import load_index
from mmxdemake.savestate import ram_from, read_state
from mmxdemake import learn as L
from mmxdemake.ramlayout import read_layout, read_size, edge_score, solid_masks, check_layout
from mmxdemake.stagemap import decode_screens, decode_tile_info
ex = Path.home() / "mmx-data/x6/extracted/retail"
sp = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
ram = ram_from(read_state(sp), (ex / "iso/SLUS_013.95").read_bytes(), sp)
recs = {r['index']: r for r in load_index(ex / 'index.json')['files'] if r['kind'] == 'rock_dat'}
b = (ex / recs[118]['path']).read_bytes()
info = entry_data(b, only(split(b), 1))
base = 0x169CE0
r = ram[base:base + len(info)]
d = [i for i in range(len(info)) if info[i] != r[i]]
print('bytes differ', len(d), 'offsets mod 4', sorted(set(i % 4 for i in d)), 'first', d[:20])
for i in d[:12]:
    print(hex(i), info[i - i % 4:i - i % 4 + 4].hex(), r[i - i % 4:i - i % 4 + 4].hex())
w, h = read_size(ram)
layout = read_layout(ram, w, h)
for m in (98, 118):
    bb = (ex / recs[m]['path']).read_bytes()
    try:
        scr = decode_screens(entry_data(bb, only(split(bb), 0)))
        inf = decode_tile_info(entry_data(bb, only(split(bb), 1)))
        print(m, 'screens', len(scr), 'edges', edge_score(layout, w, h, solid_masks(scr, inf)))
    except Exception as e:
        print(m, 'error', repr(e))
print('max screen index in layout', max(layout), 'n', len(layout))
