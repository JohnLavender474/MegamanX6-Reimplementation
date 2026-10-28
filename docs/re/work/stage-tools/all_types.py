import sys
from collections import defaultdict
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.index import load_index
from mmxdemake.arcentries import split, ArcError
ex = Path.home() / "mmx-data/x6/extracted/retail"
recs = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
where = defaultdict(list)
for r in recs:
    body = (ex / r["path"]).read_bytes()
    try:
        es = split(body)
    except ArcError as e:
        continue
    for e in es:
        where[e.type].append((r["index"], e.size))
    if r["index"] in range(123, 141):
        print(r["index"], " ".join(f"{e.type:#x}:{e.size:#x}" for e in es))
for t in sorted(where):
    if t >> 16 or t in (0x16,):
        ms = where[t]
        print(f"{t:#x}", len(ms), sorted({s for _, s in ms}), [m for m, _ in ms][:40])
