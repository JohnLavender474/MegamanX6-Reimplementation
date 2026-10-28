import json, sys
from pathlib import Path
from mmxdemake.arcentries import entry_data, only, split
from mmxdemake.stagemap import SIDE, collision, decode_screens, decode_tile_info, tile
ex = Path.home() / "mmx-data/x6/extracted/retail"
member, folder, want = int(sys.argv[1]), sys.argv[2], int(sys.argv[3])
body = (ex / f"rock_dat/{member:03d}.bin").read_bytes()
ents = split(body)
screens = decode_screens(entry_data(body, only(ents, 0)))
info = decode_tile_info(entry_data(body, only(ents, 1)))
lay = json.loads((Path.home() / "mmx-data/x6/stages" / folder / "layout.json").read_text())
cells = set()
for y, row in enumerate(lay["layers"][0]):
    for x, s in enumerate(row):
        if not s:
            continue
        for k, v in enumerate(screens[s]):
            t = tile(v)
            if t and collision(info[t]) == want:
                cells.add((x * 16 + k % SIDE, y * 16 + k // SIDE))
seen = set()
for c in sorted(cells, key=lambda q: (q[0], q[1])):
    if c in seen:
        continue
    stack, comp = [c], []
    seen.add(c)
    while stack:
        p = stack.pop(); comp.append(p)
        for d in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            q = (p[0] + d[0], p[1] + d[1])
            if q in cells and q not in seen:
                seen.add(q); stack.append(q)
    xs = [p[0] for p in comp]; ys = [p[1] for p in comp]
    print(f"x {min(xs)*16}-{max(xs)*16+15} y {min(ys)*16}-{max(ys)*16+15} cells {len(comp)} box {(max(xs)-min(xs)+1)}x{(max(ys)-min(ys)+1)}")
