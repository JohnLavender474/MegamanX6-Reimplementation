import sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.index import load_index
from mmxdemake.arcentries import split, entry_data
from mmxdemake.savestate import read_state
from mmxdemake.vram import read_vram
ex = Path.home() / "mmx-data/x6/extracted/retail"
recs = {r["index"]: r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"}
state, member = sys.argv[1], int(sys.argv[2])
vram = read_vram(read_state(Path.home() / "mmx-data/x6/dumps" / state))
body = (ex / recs[member]["path"]).read_bytes()
for k, e in enumerate(split(body)):
    if e.type >> 16 != 1 or e.type == 0x10102:
        continue
    d = entry_data(body, e)
    pages = e.size // 0x8000
    res = []
    for y0 in range(0, 512 - 255):
        for x0 in range(0, 1024, 64):
            o = y0 * 2048 + x0 * 2
            if vram[o:o + 128] == d[:128] and any(d[:128]):
                okp = sum(vram[(y0 + r) * 2048 + (x0 + p * 64) * 2:][:128] == d[p * 0x8000 + r * 128:][:128]
                          for p in range(pages) for r in range(256)) if x0 + pages * 64 <= 1024 else -1
                wid = e.size // 256
                okr = sum(vram[(y0 + r) * 2048 + x0 * 2:][:wid] == d[r * wid:(r + 1) * wid] for r in range(256)) if x0 * 2 + wid <= 2048 else -1
                res.append(((x0, y0), f"pages {okp}/{pages*256}", f"rows {okr}/256"))
    print(f"entry {k} {e.type:#x} size {e.size:#x}:", res[:6])
