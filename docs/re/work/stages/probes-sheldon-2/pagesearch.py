import sys
from pathlib import Path
from mmxdemake.arcentries import entry_data, split
from mmxdemake.index import load_index
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, ROW, PAGE_W, PAGE_ROWS, TEX_ROW

ex = Path.home() / "mmx-data/x6/extracted/retail"
recs = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
v = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / sys.argv[1]))
pages = {}
for p in range(8):
    x = 320 + p * PAGE_W
    pages[p] = b"".join(v[(256 + r) * ROW + x * 2:(256 + r) * ROW + x * 2 + TEX_ROW] for r in range(PAGE_ROWS))
    halves = [pages[p][:0x4000], pages[p][0x4000:]]
    pages[p] = halves
for r in recs:
    try:
        body = (ex / r["path"]).read_bytes()
        ents = split(body)
    except Exception:
        continue
    for e in ents:
        d = entry_data(body, e)
        for p, hs in pages.items():
            for h, chunk in enumerate(hs):
                i = d.find(chunk)
                if i >= 0:
                    print(f"page {p} half {h}: member {r['index']} entry {e.type:#x} at {i:#x} (size {len(d):#x})")
