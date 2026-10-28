import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.arcentries import split, entry_data
from mmxdemake.index import load_index
from mmxdemake.savestate import ram_from, read_state
ex = Path.home() / "mmx-data/x6/extracted/retail"
sp = Path.home() / "mmx-data/x6/dumps" / sys.argv[1]
ram = ram_from(read_state(sp), (ex / "iso/SLUS_013.95").read_bytes(), sp)
recs = {r['index']: r for r in load_index(ex / 'index.json')['files'] if r['kind'] == 'rock_dat'}
for m in map(int, sys.argv[2:]):
    b = (ex / recs[m]['path']).read_bytes()
    for i, e in enumerate(split(b)):
        d = entry_data(b, e)
        hits = []
        for k in range(0, len(d) - 64, 64):
            p = ram.find(d[k:k+64])
            if p >= 0: hits.append((k, p - k))
        starts = {}
        for k, s in hits: starts[s] = starts.get(s, 0) + 1
        top = sorted(starts.items(), key=lambda kv: -kv[1])[:3]
        print(m, i, hex(e.type), e.size, 'chunks', len(hits), '/', len(d)//64, 'bases', [(hex(0x80000000 + s), c) for s, c in top])
