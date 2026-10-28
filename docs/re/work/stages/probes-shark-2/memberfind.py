import sys, os
from pathlib import Path
from mmxdemake.index import load_index
from mmxdemake.collision import map_records
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.savestate import read_ram
X = Path(os.path.expanduser('~/mmx-data/x6/extracted/retail'))
EXE = (X / 'iso/SLUS_013.95').read_bytes()
recs = [r for r in load_index(X / 'index.json')['files'] if r['kind'] == 'rock_dat']
for st in sys.argv[1:]:
    ram = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    print('==', st)
    for r in map_records(recs):
        body = (X / r['path']).read_bytes()
        es = split(body)
        info = entry_data(body, only(es, 1))
        whole = ram.find(info)
        n = len(info) // 64
        hits = sum(1 for k in range(n) if ram.find(info[k*64:(k+1)*64]) >= 0)
        if hits > n // 10 or whole >= 0:
            print(f"  member {r['index']} info {len(info)} whole@{whole:#x} chunks {hits}/{n}", [hex(e.type) for e in es])
