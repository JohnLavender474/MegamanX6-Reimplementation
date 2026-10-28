import json, sys
from pathlib import Path
sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.index import load_index
from mmxdemake.arcentries import split, entry_data, only
from mmxdemake.stagemap import decode_screens, decode_tile_info, tile, collision
from mmxdemake.ramlayout import solid_masks, edge_score, _edges, _match

ex = Path.home() / "mmx-data/x6/extracted/retail"
cat = json.loads((Path.home() / "IdeaProjects/mmx-demake-tools/catalog/stages.json").read_text())
recs = {r["index"]: r for r in load_index(ex / "index.json")["files"] if r["kind"] == "rock_dat"}
WEAK = 0.6
seen = set()
for e in cat:
    key = (e["member"], e["overlay"], e["offset"])
    if key in seen:
        print(f"== {e['name']}: same layout as above")
        continue
    seen.add(key)
    w, h = e["width"], e["height"]
    body = (ex / recs[e["member"]]["path"]).read_bytes(); es = split(body)
    sc = decode_screens(entry_data(body, only(es, 0))); info = decode_tile_info(entry_data(body, only(es, 1)))
    masks = solid_masks(sc, info)
    ov = (ex / f"rock_bin/{e['overlay']:02d}.bin").read_bytes()
    lay = ov[e["offset"]:e["offset"] + w * h]
    empty = {s for s in range(len(sc)) if all(tile(v) == 0 for v in sc[s])}
    live = {s for s, m in enumerate(masks) if any(map(any, m)) and not all(map(all, m))}
    weak = []
    for y in range(h):
        for x in range(w):
            a = lay[y * w + x]
            if a not in live:
                continue
            if x + 1 < w and lay[y * w + x + 1] in live:
                s = _match(_edges(masks[a])[0], _edges(masks[lay[y * w + x + 1]])[1])
                if s < WEAK:
                    weak.append(("h", x, y, round(s, 2)))
            if y + 1 < h and lay[(y + 1) * w + x] in live:
                b = lay[(y + 1) * w + x]
                ea, eb = _edges(masks[a])[2], _edges(masks[b])[3]
                s = _match(ea, eb)
                if s < WEAK and len(set(ea)) > 1 and len(set(eb)) > 1:
                    weak.append(("v", x, y, round(s, 2)))
    holes = []
    for y in range(h):
        for x in range(w):
            v = lay[y * w + x]
            if v != 0 and v not in empty:
                continue
            n = [lay[yy * w + xx] for xx, yy in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)) if 0 <= xx < w and 0 <= yy < h]
            full = [s for s in n if s and s not in empty]
            if len(full) >= 3 and (0 < x < w - 1) and (0 < y < h - 1):
                holes.append((x, y))
    shifts = []
    for k in list(range(-40, 41)):
        o = e["offset"] + k
        l = ov[o:o + w * h]
        if len(l) < w * h or any(v >= len(sc) for v in l):
            continue
        hs, hn, vs, vn, _ = edge_score(l, w, h, masks)
        shifts.append((round(hs * hn + vs * vn, 1), k, round(hs, 3), hn, round(vs, 3), vn))
    shifts.sort(reverse=True)
    hs, hn, vs, vn, pairs = edge_score(lay, w, h, masks)
    row0 = sum(1 for x in range(w) if lay[x] in live)
    print(f"== {e['name']} m{e['member']} {w}x{h}: h {hs:.3f}/{hn} v {vs:.3f}/{vn} (pairs {pairs}); "
          f"live screens in row 0: {row0}/{w}")
    print(f"   weak joins (<{WEAK}): {weak}")
    print(f"   holes (empty cell with >=3 full neighbours, inside): {holes}")
    print(f"   best shifts (matched joins, k, h, hn, v, vn): {shifts[:3]}")
