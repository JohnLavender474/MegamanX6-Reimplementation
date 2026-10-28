import sys, hashlib
from pathlib import Path
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake import rekit
from mmxdemake.codewalk import walk, references, self_fields
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, "shark-1_ride.state.gz")
ram = bytearray(ram)
ov = (Path.home()/"mmx-data/x6/extracted/retail/rock_bin/28.bin").read_bytes()
base = 0x800E9860
ram[base-0x80000000: base-0x80000000+len(ov)] = ov
for i in range(len(ov), 42944): ram[base-0x80000000+i] = 0
ram = bytes(ram)
regions = [(base, base+len(ov))] + [r for r in regions if r[0] < 0x800E0000]
helpers = rekit.load_helpers(rekit.DEFAULT_CATALOG)
root = int(sys.argv[1], 16); out = Path(sys.argv[2])
graph = walk(ram, [root], [regions[0]], regions, helpers["functions"])
ctx = {"ram": ram, "helpers": helpers, "graph": graph, "label": "overlay 28 " + sys.argv[1], "state": "overlay 28 file over shark-1_ride", "cls": "actor2",
       "type": 1, "entry": root, "stage": 20, "area": 1, "regions": regions, "sha1": "-", "neighbours": [], "unwalked": [],
       "main": rekit.main_functions(graph, root), "animations": {}, "references": references(ram, graph, regions),
       "fields": {f: self_fields(ram, f, end) for f, end in graph.functions.items()}}
out.mkdir(parents=True, exist_ok=True)
for n, fn in (("summary.md", rekit.summary_text), ("code.txt", rekit.code_text), ("calls.txt", rekit.call_text)):
    (out / n).write_text(fn(ctx))
print(len(graph.functions), "functions", len(graph.tables), "tables")
