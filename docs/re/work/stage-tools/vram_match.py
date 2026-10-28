import sys, hashlib
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.index import load_index
from mmxdemake.collision import map_records
from mmxdemake.arcentries import split, entry_data
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import stage_area
from mmxdemake.vram import read_vram, pages_from_rows, _differing_rows, VRAM_W, VRAM_H, PAGES, PAGE_W, PAGE_ROWS

ex = Path.home() / "mmx-data/x6/extracted/retail"
dumps = Path.home() / "mmx-data/x6/dumps"
exe = (ex / "iso/SLUS_013.95").read_bytes()
recs = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
blocks = {}
for r in map_records(recs):
    body = (ex / r["path"]).read_bytes()
    for k, e in enumerate(split(body)):
        if e.size == 0x40000:
            d = entry_data(body, e)
            blocks[(r["index"], e.type, k, "pages")] = d
            blocks[(r["index"], e.type, k, "rows")] = pages_from_rows(d)
only = sys.argv[1:] or [p.name for p in sorted(dumps.glob("*.state.gz"))]
for name in only:
    st = dumps / name
    data = read_state(st)
    ram = ram_from(data, exe, st)
    vram = read_vram(data)
    s, a = stage_area(ram)
    best = []
    for key, b in blocks.items():
        for y0 in (0, VRAM_H - PAGE_ROWS):
            for x0 in range(0, VRAM_W - PAGES * PAGE_W + 1, PAGE_W):
                bad = _differing_rows(vram, b, x0, y0)
                if bad < 2048 * 0.25:
                    best.append((bad, key, (x0, y0)))
    best.sort()
    print(name, f"stage {s} area {a}", [(b, f"m{k[0]} {k[1]:#x} {k[3]}", p) for b, k, p in best[:4]])
