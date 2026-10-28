import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake import rekit
from mmxdemake.codewalk import is_function_start
DUMPS = Path.home()/"mmx-data/x6/dumps"; EXT = Path.home()/"mmx-data/x6/extracted/retail"
for state in ("intro-1_start.state.gz", "turtloid-6_zero-player.state.gz"):
    path, ram, scratch, regions, info = rekit._load(DUMPS, EXT, state, player=True)
    h = rekit.load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")["functions"]
    g, roles, steps = rekit.player_graph(ram, regions, info, h)
    n, lo, hi, prog = info
    called_from_main = {}
    for f, cs in g.calls.items():
        if prog[0] <= f < prog[1]:
            for c in cs:
                if lo <= c < hi: called_from_main.setdefault(c, set()).add(f)
    bad = [c for c in called_from_main if not is_function_start(ram, c)]
    print(state, "overlay targets called from main:", len(called_from_main), "not a function start:", len(bad), [hex(b) for b in sorted(bad)][:20])
    print("   beyond loaded overlay end?", [hex(c) for c in called_from_main if c >= hi][:5])
