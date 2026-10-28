import sys, struct, json
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake import rekit
from mmxdemake.codewalk import function_end
from mmxdemake.mips import jump_target, s16
DUMPS = Path.home()/"mmx-data/x6/dumps"; EXT = Path.home()/"mmx-data/x6/extracted/retail"
state = sys.argv[1]
path, ram, scratch, regions, info = rekit._load(DUMPS, EXT, state, player=True)
helpers = rekit.load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")
graph, roles, steps = rekit.player_graph(ram, regions, info, helpers["functions"])
n, lo, hi, prog = info
u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
SETANIME = {0x800179A4, 0x800179D0}
def scan(f):
    end = graph.functions.get(f) or function_end(ram, f)
    const, anims, stepw, subw, ovcalls, jalr = {}, set(), set(), set(), set(), 0
    pcs = list(range(f, end, 4))
    i = 0
    while i < len(pcs):
        pc = pcs[i]; x = u32(pc); op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
        t = jump_target(x, pc)
        if t is not None and op == 3:
            d = u32(pc + 4); dop = d >> 26
            if dop in (9, 13) and (d >> 21) & 31 == 0: const[(d >> 16) & 31] = s16(d) if dop == 9 else d & 0xFFFF
            if t in SETANIME and 5 in const: anims.add(const[5])
            if lo <= t < hi: ovcalls.add(t)
            i += 2; continue
        if op == 0 and x & 63 == 9: jalr += 1
        if op in (9, 13) and rs == 0: const[rt] = s16(x) if op == 9 else x & 0xFFFF
        elif op == 0x28 and s16(x) in (5, 6) and rt in const:
            (stepw if s16(x) == 5 else subw).add(const[rt])
        else:
            w = (x >> 11) & 31 if op == 0 else (rt if op not in (0x28, 0x29, 0x2B, 4, 5, 1, 2, 3) else None)
            if w in const and not (op in (9, 13) and rs == 0): const.pop(w, None)
        i += 1
    return anims, stepw, subw, ovcalls, jalr
def reach(f, depth=3, seen=None):
    seen = seen if seen is not None else set()
    if f in seen or depth < 0: return seen
    seen.add(f)
    for c in graph.calls.get(f, ()):
        if prog[0] <= c < prog[1] and c in graph.functions and c not in helpers["functions"]:
            reach(c, depth - 1, seen)
    return seen
counts = {}
for k, f in enumerate(steps):
    for g in reach(f): counts[g] = counts.get(g, 0) + 1
out = {}
for k, f in enumerate(steps):
    fs = [g for g in reach(f) if counts[g] <= 3 or g == f]
    A, S, SS, O, J = set(), set(), set(), set(), 0
    for g in fs:
        a, s, ss, o, j = scan(g); A |= a; S |= s; SS |= ss; O |= o; J += j
    out[k] = {"fn": hex(f), "funcs": len(fs), "anims": sorted(A), "to_steps": sorted(S), "substeps": sorted(SS), "overlay_calls": sorted(hex(o) for o in O), "jalr": J}
    print(f"step {k:2} {f:#x} funcs {len(fs):2} anims {sorted(A)} -> steps {sorted(S)} sub {sorted(SS)} ov {len(O)} jalr {J}")
json.dump(out, open(Path(sys.argv[2]), "w"), indent=1)
