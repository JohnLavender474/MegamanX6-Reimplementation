import sys, hashlib
from pathlib import Path
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from mmxdemake import rekit, outdir
from mmxdemake.codewalk import walk, references, self_fields
state = sys.argv[1]; roots = [int(x, 16) for x in sys.argv[2].split(",")]; out = Path(sys.argv[3]); label = sys.argv[4]
helpers = rekit.load_helpers(rekit.DEFAULT_CATALOG)
path, ram, scratch, regions = rekit._load(rekit.DEFAULT_DUMPS, rekit.DEFAULT_EXTRACTED, state)
expand = next(((a, b) for a, b in regions if a <= roots[0] < b))
graph = walk(ram, roots, [expand], regions, helpers["functions"])
stage, area = rekit.stage_area(ram)
ctx = {"ram": ram, "helpers": helpers, "graph": graph, "label": label, "state": state, "cls": "actor2",
       "type": 0, "entry": roots[0], "stage": stage, "area": area, "regions": regions,
       "sha1": hashlib.sha1(path.read_bytes()).hexdigest(), "neighbours": [], "unwalked": [],
       "main": rekit.main_functions(graph, roots[0]), "animations": {},
       "references": references(ram, graph, regions),
       "fields": {f: self_fields(ram, f, end) for f, end in graph.functions.items()}}
out.mkdir(parents=True, exist_ok=True)
for n, fn in (("summary.md", rekit.summary_text), ("code.txt", rekit.code_text), ("calls.txt", rekit.call_text), ("data.txt", rekit.data_text)):
    (out / n).write_text(fn(ctx))
print(len(graph.functions), "functions", len(graph.tables), "tables", regions)
