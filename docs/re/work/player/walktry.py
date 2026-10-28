import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.animfind import program_bounds, player_overlay
from mmxdemake.rekit import load_helpers
import mmxdemake.codewalk as C
X = Path.home()/"mmx-data/x6/extracted/retail"; exe = (X/"iso/SLUS_013.95").read_bytes()
ov = {int(p.stem): p.read_bytes() for p in sorted((X/"rock_bin").glob("*.bin"))}
p = Path.home()/"mmx-data/x6/dumps"/sys.argv[1]; r = ram_from(read_state(p), exe, p)
h = load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")["functions"]
u32 = lambda a: struct.unpack_from("<I", r, a - 0x80000000)[0]
n, plo, phi = player_overlay(r, ov); mlo, mhi = program_bounds(exe)
roots = [0x80034FBC] + [u32(0x80073ACC + 4*i) for i in range(32)]
for name, span in (("main", (mlo, mhi)), ("overlay", (plo, phi))):
    g = C.walk(r, roots, span, [(mlo, mhi), (plo, phi)], h, limit=3000)
    fs = sorted(g.functions)
    ino = sum(1 for f in fs if plo <= f < phi)
    print(name, "functions", len(fs), "in overlay", ino, "truncated", g.truncated, "range", hex(fs[0]), hex(fs[-1]), "leaves", len(g.leaves), "tables", len(g.tables))
    print("  leaves in overlay:", len([l for l in g.leaves if plo <= l < phi]), " leaves in main:", len([l for l in g.leaves if mlo <= l < mhi]))
