import sys, struct, json
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake import rekit
from mmxdemake.codewalk import self_fields
from mmxdemake.mips import jump_target, s16
DUMPS = Path.home()/"mmx-data/x6/dumps"; EXT = Path.home()/"mmx-data/x6/extracted/retail"
state, outp = sys.argv[1], sys.argv[2]
path, ram, scratch, regions, info = rekit._load(DUMPS, EXT, state, player=True)
H = rekit.load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")
g, roles, steps, foreign = rekit.player_graph(ram, regions, info, H["functions"])
n, lo, hi, prog = info
u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
SET = {0x800179A4: "SetAnime", 0x800179D0: "SetAnime2", 0x8003BA04: "SetAnime", 0x8003BB94: "SetAnime2", 0x80016C48: "PlaySound"}
def facts(f):
    end = g.functions[f]; _, notes = self_fields(ram, f, end)
    const = {}; out = {"step": set(), "sub": set(), "anim": set(), "sound": set(), "calls": [], "var_test": False}
    pc = f
    while pc < end:
        x = u32(pc); op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
        if op == 3:
            d = u32(pc + 4)
            if d >> 26 in (9, 13) and (d >> 21) & 31 == 0: const[(d >> 16) & 31] = s16(d) if d >> 26 == 9 else d & 0xFFFF
            t = jump_target(x, pc)
            nm = SET.get(t)
            if nm in ("SetAnime", "SetAnime2") and 5 in const: out["anim"].add(const[5])
            if nm == "PlaySound" and 5 in const: out["sound"].add(const[5])
            out["calls"].append(t)
            for r in (2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 24, 25): const.pop(r, None)
            pc += 8; continue
        if op in (9, 13) and rs == 0:
            const[rt] = s16(x) if op == 9 else x & 0xFFFF
        elif op == 0x28 and pc in notes and notes[pc] in (5, 6):
            if rt in const: out["step" if notes[pc] == 5 else "sub"].add(const[rt])
            elif rt == 0: out["step" if notes[pc] == 5 else "sub"].add(0)
        elif op in (0x20, 0x24) and pc in notes and notes[pc] == 2:
            out["var_test"] = True
        else:
            w = (x >> 11) & 31 if op == 0 else (rt if op in (8, 9, 10, 11, 12, 13, 14, 15, 0x20, 0x21, 0x23, 0x24, 0x25) else None)
            if w is not None: const.pop(w, None)
        pc += 4
    return out
F = {f: facts(f) for f in g.functions}
def area(f): return "ov" if lo <= f < hi else "main"
doc = {}
for f, v in F.items():
    doc[hex(f)] = {"area": area(f), "role": roles.get(f, ""), "end": hex(g.functions[f]),
                   "step": sorted(v["step"]), "sub": sorted(v["sub"]), "anim": sorted(v["anim"]), "sound": sorted(v["sound"]),
                   "calls": [hex(c) for c in v["calls"]], "var_test": v["var_test"]}
json.dump(doc, open(outp, "w"), indent=1)
print("functions", len(doc), "with step writes", sum(1 for v in doc.values() if v["step"]), "with anims", sum(1 for v in doc.values() if v["anim"]))
