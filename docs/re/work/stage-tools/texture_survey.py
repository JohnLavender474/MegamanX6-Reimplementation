import json, sys, hashlib
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.index import load_index
from mmxdemake.collision import map_records
from mmxdemake.arcentries import split

ex = Path.home() / "mmx-data/x6/extracted/retail"
recs = [r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"]
for r in map_records(recs):
    body = (ex / r["path"]).read_bytes()
    es = split(body)
    print(r["index"], " ".join(f"{e.type:#x}:{e.size:#x}" for e in es))
