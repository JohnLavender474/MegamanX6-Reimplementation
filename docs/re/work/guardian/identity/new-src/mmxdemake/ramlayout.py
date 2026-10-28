import struct

from mmxdemake.savestate import ram_at
from mmxdemake.stagemap import SIDE, collision, tile

LAYOUT_WIDTH = 0x800CD338
LAYOUT_HEIGHT = 0x800CD339
LAYOUT_SIZE = 0x8008EC10
LAYOUT_RAM = 0x80097438
LAYERS = 3
THRESHOLD = 0.85
V_THRESHOLD = 0.70
MIN_H_JOINS = 10
MIN_V_JOINS = 3


class LayoutError(Exception):
    pass


def read_size(ram):
    w = ram_at(ram, LAYOUT_WIDTH, 1)[0]
    h = ram_at(ram, LAYOUT_HEIGHT, 1)[0]
    size = struct.unpack("<H", ram_at(ram, LAYOUT_SIZE, 2))[0]
    if not w or not h or size != w * h:
        raise LayoutError(f"layout size fields width {w}, height {h}, size {size} do not agree")
    return w, h


def find_member(ram, tile_infos, fits=None):
    found = sorted(m for m, info in tile_infos.items() if info and ram.find(info) >= 0)
    if len(found) > 1 and fits is not None:
        fitting = [m for m in found if fits(m)]
        if len(fitting) != 1:
            raise LayoutError(f"{len(found)} stage members have their tile info in RAM ({found}) and "
                              f"{len(fitting)} fit the layout ({fitting}); need exactly 1")
        return fitting[0]
    if len(found) != 1:
        raise LayoutError(f"{len(found)} stage members have their tile info in RAM ({found}); need exactly 1")
    return found[0]


def read_layout(ram, w, h):
    return ram_at(ram, LAYOUT_RAM, LAYERS * w * h)


def solid_masks(screens, info):
    return [[[1 if collision(info[tile(s[y * SIDE + x])]) else 0 for x in range(SIDE)] for y in range(SIDE)]
            for s in screens]


def _edges(mask):
    return (tuple(row[-1] for row in mask), tuple(row[0] for row in mask), tuple(mask[-1]), tuple(mask[0]))


def _match(a, b):
    return sum(1 for p, q in zip(a, b) if p == q) / len(a)


def edge_score(layout, w, h, masks):
    live = {s for s, m in enumerate(masks) if any(map(any, m)) and not all(map(all, m))}
    edges = {s: _edges(masks[s]) for s in live}
    hs = vs = 0.0
    hn = vn = pairs = 0
    for y in range(h):
        for x in range(w):
            a = layout[y * w + x]
            if a not in live:
                continue
            if x + 1 < w:
                b = layout[y * w + x + 1]
                if b in live:
                    hs += _match(edges[a][0], edges[b][1])
                    hn += 1
            if y + 1 < h:
                b = layout[(y + 1) * w + x]
                if b in live:
                    pairs += 1
                    if len(set(edges[a][2])) > 1 and len(set(edges[b][3])) > 1:
                        vs += _match(edges[a][2], edges[b][3])
                        vn += 1
    return (hs / hn if hn else 0.0), hn, (vs / vn if vn else 0.0), vn, pairs


def check_layout(layout, w, h, masks, screen_count):
    bad = [k for k, v in enumerate(layout) if v >= screen_count]
    if bad:
        k = bad[0]
        raise LayoutError(f"{len(bad)} layout cells are not screen numbers (first: layer {k // (w * h)}, "
                          f"x {(k % (w * h)) % w}, y {(k % (w * h)) // w}, value {layout[k]}); "
                          f"the fixed RAM address {LAYOUT_RAM:#x} may be wrong for this stage")
    hsc, hj, vsc, vj, pairs = edge_score(layout, w, h, masks)
    if hj < MIN_H_JOINS or hsc < THRESHOLD or (pairs >= MIN_V_JOINS and (vj < MIN_V_JOINS or vsc < V_THRESHOLD)):
        raise LayoutError(f"screens do not join: horizontal {hsc:.3f} over {hj} joins, vertical {vsc:.3f} "
                          f"over {vj} joins; the fixed RAM address {LAYOUT_RAM:#x} may be wrong for this stage")
    return hsc, hj, vsc, vj


def _all_hits(data, needle):
    k = data.find(needle)
    while k >= 0:
        yield k
        k = data.find(needle, k + 1)


def find_on_disc(layout, w, overlays):
    needle = bytes(layout[w:])
    hits = [(i, k - w) for i, data in sorted(overlays.items()) for k in _all_hits(data, needle) if k >= w]
    if len(hits) != 1:
        raise LayoutError(f"layout found {len(hits)} times in the overlays; need exactly 1")
    return hits[0]


def differing_cells(disc, ram_layout, w, h):
    return [(k // (w * h), (k % (w * h)) % w, (k % (w * h)) // w)
            for k in range(len(ram_layout)) if disc[k] != ram_layout[k]]
