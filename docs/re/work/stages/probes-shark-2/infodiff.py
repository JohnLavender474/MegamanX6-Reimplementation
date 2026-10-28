import sys, os, struct
from pathlib import Path
from mmxdemake.index import load_index
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.savestate import read_ram
from mmxdemake.stagemap import decode_tile_info
X = Path(os.path.expanduser('~/mmx-data/x6/extracted/retail'))
EXE = (X / 'iso/SLUS_013.95').read_bytes()
r = next(r for r in load_index(X / 'index.json')['files'] if r['kind'] == 'rock_dat' and r['index'] == 99)
body = (X / r['path']).read_bytes()
info = entry_data(body, only(split(body), 1))
print('info len', len(info))
for st in sys.argv[1:]:
    ram = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    base = 0x1b0800
    seg = ram[base:base + len(info)]
    d = [k for k in range(len(info)) if seg[k] != info[k]]
    print('==', st, len(d), 'bytes differ')
    for k in d[:80]:
        print(f'   +{k:#06x} (word {k//4}, ram {0x80000000+base+k:#x}) disc {info[k]:02x} ram {seg[k]:02x}')
