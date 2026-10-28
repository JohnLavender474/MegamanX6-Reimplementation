import sys
from pathlib import Path
from mmxdemake.arcentries import entry_data, split
from mmxdemake.index import load_index
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW, PAGE_W, PAGE_ROWS, PAGE_BYTES, TEX_ROW, PAGES

ex = Path.home() / "mmx-data/x6/extracted/retail"
recs = {r["index"]: r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"}
body = (ex / recs[102]["path"]).read_bytes()
ents = split(body)
print("entries", [(hex(e.type) if hasattr(e, "type") else e) for e in ents][:40])
tex = {}
for e in ents:
    t = e[0] if isinstance(e, tuple) else getattr(e, "type", None)
    tex.setdefault(t, entry_data(body, e))
print({hex(k) if isinstance(k, int) else k: len(v) for k, v in tex.items()})
block = tex[int(sys.argv[1], 16)]
for st in sys.argv[2:]:
    v = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / st))
    out = []
    for p in range(PAGES):
        bad = 0
        for r in range(PAGE_ROWS):
            o = (256 + r) * ROW + (320 + p * PAGE_W) * 2
            s = p * PAGE_BYTES + r * TEX_ROW
            if v[o:o + TEX_ROW] != block[s:s + TEX_ROW]:
                bad += 1
        out.append(bad)
    print(st, out)
