import sys, os
from collections import Counter
from pathlib import Path
from mmxdemake.index import load_index
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.savestate import read_ram
from mmxdemake.stagemap import decode_tile_info, decode_screens
from mmxdemake.ramlayout import read_layout
from mmxdemake import palettes as P
X = Path(os.path.expanduser('~/mmx-data/x6/extracted/retail'))
EXE = (X / 'iso/SLUS_013.95').read_bytes()
r = next(r for r in load_index(X / 'index.json')['files'] if r['kind'] == 'rock_dat' and r['index'] == 99)
body = (X / r['path']).read_bytes()
es = split(body)
info = decode_tile_info(entry_data(body, only(es, 1)))
screens = decode_screens(entry_data(body, only(es, 0)))
for st in sys.argv[1:]:
    ram = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    layers = P.layers_from(read_layout(ram, 26, 5), 26, 5)
    sp = P.sprites(ram)
    print('==', st, 'sprite cmds', len(sp), 'distinct', len(set(sp)))
    for li, layout in enumerate(layers):
        cells = P._cells(layout, 26, 5, screens, info)
        votes = Counter()
        for sx, sy, cu, cv, _, _ in set(sp):
            for px, py in cells.get((cu, cv), ()):
                votes[(px - sx, py - sy)] += 1
        print('  layer', li, votes.most_common(5))
