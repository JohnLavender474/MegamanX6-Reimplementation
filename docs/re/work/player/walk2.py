import sys, struct
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
from mmxdemake.animfind import program_bounds, player_overlay
from mmxdemake.rekit import load_helpers
import mmxdemake.codewalk as C
X = Path.home()/"mmx-data/x6/extracted/retail"; exe = (X/"iso/SLUS_013.95").read_bytes()
ov = {int(p.stem): p.read_bytes() for p in sorted((X/"rock_bin").glob("*.bin"))}
p = Path.home()/"mmx-data/x6/dumps"/sys.argv[1]; d = read_state(p); r = ram_from(d, exe, p); sc = scratchpad(d)
h = load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")["functions"]
u32 = lambda a: struct.unpack_from("<I", r, a - 0x80000000)[0]
n, plo, phi = player_overlay(r, ov); mlo, mhi = program_bounds(exe)
roots = [0x80034FBC] + [u32(0x80073ACC + 4*i) for i in range(32)]
spans = [(mlo, mhi), (plo, phi)]
g = C.walk(r, roots, spans, spans, h, limit=3000)
fs = sorted(g.functions)
print("functions", len(fs), "overlay", sum(plo <= f < phi for f in fs), "main", sum(mlo <= f < mhi for f in fs), "tables", len(g.tables), "leaves", len(g.leaves))
starts = [a for a in range(plo, phi, 4) if (u32(a) >> 16) == 0x27BD and u32(a) & 0x8000]
missing = [a for a in starts if a not in g.functions]
print("overlay prologues", len(starts), "not walked", len(missing))
# who points at missing functions? scan whole RAM for words equal to them
ramw = {}
for a in range(0x80000000, 0x80200000, 4):
    w = u32(a)
    if plo <= w < phi: ramw.setdefault(w, []).append(a)
for m in missing[:200]:
    refs = ramw.get(m, [])
    print(hex(m), "pointed to from", [hex(x) for x in refs[:4]])
# weapon class table? scan scratchpad words pointing at tables containing overlay functions
for off in range(0, 0x100, 4):
    t = struct.unpack_from("<I", sc, off)[0]
    if 0x80000000 <= t < 0x80200000:
        ws = [u32(t + 4*i) for i in range(8)]
        if any(plo <= w < phi for w in ws): print("scratch +%#x -> %#x" % (off, t), [hex(w) for w in ws])
