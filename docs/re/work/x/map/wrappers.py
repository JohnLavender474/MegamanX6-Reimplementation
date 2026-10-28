import sys, struct, collections
sys.path.insert(0, "/home/johnlavender/IdeaProjects/mmx-demake-tools")
from pathlib import Path
from mmxdemake import rekit
from mmxdemake.mips import jump_target, s16
DUMPS = Path.home()/"mmx-data/x6/dumps"; EXT = Path.home()/"mmx-data/x6/extracted/retail"
path, ram, scratch, regions, info = rekit._load(DUMPS, EXT, sys.argv[1], player=True)
H = rekit.load_helpers("/home/johnlavender/IdeaProjects/mmx-demake-tools/catalog")
g, roles, steps, foreign = rekit.player_graph(ram, regions, info, H["functions"])
u32 = lambda a: struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]
cnt = collections.Counter(); vals = collections.defaultdict(set)
for f, end in g.functions.items():
    const = {}
    for pc in range(f, end, 4):
        x = u32(pc); op = x >> 26
        if op in (9, 13) and (x >> 21) & 31 == 0: const[(x >> 16) & 31] = s16(x) if op == 9 else x & 0xFFFF
        if op == 3:
            d = u32(pc + 4)
            c = dict(const)
            if d >> 26 in (9, 13) and (d >> 21) & 31 == 0: c[(d >> 16) & 31] = s16(d) if d >> 26 == 9 else d & 0xFFFF
            t = jump_target(x, pc)
            if 5 in c: cnt[t] += 1; vals[t].add(c[5])
            const = {}
for t, k in cnt.most_common(15):
    nm = H["functions"].get(t, ("",))[0]
    print(hex(t), nm, k, sorted(vals[t])[:25])
