import json, sys
from collections import Counter, defaultdict
from pathlib import Path
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.stagemap import SIDE, collision, decode_screens, decode_tile_info, tile

ex = Path.home() / "mmx-data/x6/extracted/retail"
member, folder = int(sys.argv[1]), sys.argv[2]
body = (ex / f"rock_dat/{member:03d}.bin").read_bytes()
ents = split(body)
screens = decode_screens(entry_data(body, only(ents, 0)))
info = decode_tile_info(entry_data(body, only(ents, 1)))
lay = json.loads((Path.home() / "mmx-data/x6/stages" / folder / "layout.json").read_text())
l0 = lay["layers"][0]
count = Counter()
where = defaultdict(list)
used = set()
for y, row in enumerate(l0):
    for x, s in enumerate(row):
        if not s:
            continue
        used.add((x, y))
        for k, v in enumerate(screens[s]):
            t = tile(v)
            c = 0 if t == 0 else collision(info[t])
            count[c] += 1
            px, py = x * 256 + (k % SIDE) * 16, y * 256 + (k // SIDE) * 16
            where[c].append((px, py))
print("used screens", sorted(used))
print(sorted(count.items()))
for c, pts in sorted(where.items()):
    if c in (0, 56):
        continue
    xs = sorted(set(p[0] for p in pts)); ys = sorted(set(p[1] for p in pts))
    runs = []
    for p in sorted(pts, key=lambda q: (q[1], q[0])):
        if runs and runs[-1][1] == p[1] and runs[-1][2] + 16 == p[0]:
            runs[-1][2] = p[0]
        else:
            runs.append([p[0], p[1], p[0]])
    print(c, len(pts), "runs (x0, y, x1):", [(a, b, e + 15) for a, b, e in runs][:40])
