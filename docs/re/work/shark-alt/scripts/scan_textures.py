import sys, hashlib
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.arcentries import split, entry_data
from mmxdemake.index import load_index
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram, _differing_rows, VRAM_H, VRAM_W, PAGE_ROWS, PAGES, PAGE_W, pages_from_rows
from mmxdemake.texture import BLOCK_SIZE
ex = Path.home() / "mmx-data/x6/extracted/retail"
vram = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / sys.argv[1]))
recs = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
for r in recs:
    body = (ex / r["path"]).read_bytes()
    try:
        ents = split(body)
    except Exception:
        continue
    for e in ents:
        if e.size != BLOCK_SIZE:
            continue
        data = entry_data(body, e)
        for name, blk in (("pages", data), ("rows", pages_from_rows(data))):
            best = min(((_differing_rows(vram, blk, x0, y0), x0, y0) for y0 in (0, VRAM_H - PAGE_ROWS)
                        for x0 in range(0, VRAM_W - PAGES * PAGE_W + 1, PAGE_W)))
            if best[0] < 1500:
                print(r["index"], hex(e.type), name, best)
