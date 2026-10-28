import json, sys
from collections import Counter, defaultdict
from pathlib import Path
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.stagemap import SIDE, collision, decode_screens, decode_tile_info, tile
ex = Path.home() / "mmx-data/x6/extracted/retail"
body = (ex / "rock_dat/099.bin").read_bytes()
ents = split(body)
screens = decode_screens(entry_data(body, only(ents, 0)))
info = decode_tile_info(entry_data(body, only(ents, 1)))
lay = json.loads((Path.home() / "mmx-data/x6/stages/probes-shark-2/render/m099-metal-shark-player-section-2/layout.json").read_text())
L0 = lay["layers"][0]
W, H = len(L0[0]) * 16, len(L0) * 16
grid = [[0] * W for _ in range(H)]
for y, row in enumerate(L0):
    for x, s in enumerate(row):
        for k, v in enumerate(screens[s]):
            t = tile(v)
            grid[y * 16 + k // SIDE][x * 16 + k % SIDE] = collision(info[t]) if t else 0
c = Counter(v for r in grid for v in r)
print('values', sorted(c.items()))
print('layout layer0 rows:')
for row in L0: print('  ', row)
def comps(val):
    cells = {(x, y) for y in range(H) for x in range(W) if grid[y][x] == val}
    seen = set(); out = []
    for c0 in sorted(cells):
        if c0 in seen: continue
        st = [c0]; seen.add(c0); comp = []
        while st:
            p = st.pop(); comp.append(p)
            for d in ((1,0),(-1,0),(0,1),(0,-1)):
                q = (p[0]+d[0], p[1]+d[1])
                if q in cells and q not in seen: seen.add(q); st.append(q)
        xs = [p[0] for p in comp]; ys = [p[1] for p in comp]
        out.append((min(xs)*16, max(xs)*16+15, min(ys)*16, max(ys)*16+15, len(comp)))
    return out
for v in sorted(c):
    if v in (0, 56): continue
    cs = comps(v)
    print('value', v, len(cs), 'parts:', cs[:40])
for col in sys.argv[1:]:
    x = int(col) // 16
    print('column x', col, [(y*16, grid[y][x]) for y in range(H) if grid[y][x] and (y == 0 or grid[y-1][x] != grid[y][x])])
