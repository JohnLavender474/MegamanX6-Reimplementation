import struct
from collections import Counter, defaultdict

from mmxdemake.psxcolor import to_rgba
from mmxdemake.stagemap import SIDE, tile
from mmxdemake.texture import TILE, bits, tile_ref

SPRITE_CMDS = range(0x7C, 0x80)
MIN_VOTES = 50
VOTE_MARGIN = 1.5
MIN_MATCHES = 50
MIN_AGREE = 0.95
SCREEN_PX = SIDE * TILE
SCREEN_LIMIT = 512
VRAM_W = 1024
VRAM_H = 512
ROW = 2048
LAYERS = 3


class PaletteError(Exception):
    pass


def _signed(v):
    return v - 0x10000 if v >= 0x8000 else v


def sprites(ram):
    words = struct.unpack_from(f"<{len(ram) // 4}I", ram)
    out = []
    for i in range(len(words) - 2):
        if words[i] >> 24 not in SPRITE_CMDS:
            continue
        pos, tex = words[i + 1], words[i + 2]
        sx, sy = _signed(pos & 0xFFFF), _signed(pos >> 16)
        u, v, clut = tex & 0xFF, (tex >> 8) & 0xFF, tex >> 16
        clx, cly = (clut & 63) * 16, clut >> 6
        if u % TILE or v % TILE or cly >= VRAM_H:
            continue
        if not (-TILE < sx < SCREEN_LIMIT and -TILE < sy < SCREEN_LIMIT):
            continue
        out.append((sx, sy, u // TILE, v // TILE, clx, cly))
    return out


def layers_from(flat, w, h):
    return [[[flat[l * w * h + y * w + x] for x in range(w)] for y in range(h)] for l in range(LAYERS)]


def _cells(layout, w, h, screens, info):
    cells = defaultdict(list)
    for y in range(h):
        for x in range(w):
            s = layout[y][x]
            if not s or s >= len(screens):
                continue
            for k, v in enumerate(screens[s]):
                t = tile(v)
                if t and t < len(info):
                    r = tile_ref(info[t])
                    cells[(r.cx, r.cy)].append((x * SCREEN_PX + (k % SIDE) * TILE, y * SCREEN_PX + (k // SIDE) * TILE))
    return cells


def _tile_at(layout, w, h, screens, px, py):
    if px % TILE or py % TILE or not (0 <= px < w * SCREEN_PX and 0 <= py < h * SCREEN_PX):
        return 0
    s = layout[py // SCREEN_PX][px // SCREEN_PX]
    if not s or s >= len(screens):
        return 0
    return tile(screens[s][(py % SCREEN_PX) // TILE * SIDE + (px % SCREEN_PX) // TILE])


def cameras(sp, layers, w, h, screens, info):
    result = []
    distinct = set(sp)
    for layout in layers:
        cells = _cells(layout, w, h, screens, info)
        votes = Counter()
        for sx, sy, cu, cv, _, _ in distinct:
            for px, py in cells.get((cu, cv), ()):
                votes[(px - sx, py - sy)] += 1
        top = votes.most_common(2)
        ok = bool(top) and top[0][1] >= MIN_VOTES and (len(top) == 1 or top[0][1] > VOTE_MARGIN * top[1][1])
        result.append(top[0][0] if ok else None)
    if all(c is None for c in result):
        raise PaletteError("no camera for any layout layer: the draw commands do not match the stage")
    return result


def _used(layers, screens, info):
    used = set()
    for layout in layers:
        for row in layout:
            for s in row:
                if not s or s >= len(screens):
                    continue
                for v in screens[s]:
                    t = tile(v)
                    if t and t < len(info):
                        r = tile_ref(info[t])
                        depth = bits(r.page)
                        if depth:
                            used.add((r.palette, depth))
    return sorted(used)


def learn_rule(sp, cams, layers, w, h, screens, info):
    cands = Counter()
    for sx, sy, cu, cv, clx, cly in set(sp):
        for layer, cam in enumerate(cams):
            if cam is None:
                continue
            t = _tile_at(layers[layer], w, h, screens, cam[0] + sx, cam[1] + sy)
            if not t or t >= len(info):
                continue
            r = tile_ref(info[t])
            if (r.cx, r.cy) == (cu, cv):
                cands[(clx - (r.palette % 16) * 16, cly - r.palette // 16)] += 1
                break
    n = sum(cands.values())
    if n < MIN_MATCHES:
        raise PaletteError(f"{n} sprites match a stage tile; need at least {MIN_MATCHES}")
    (bx, by), top = cands.most_common(1)[0]
    agree = top / n
    if agree < MIN_AGREE:
        raise PaletteError(f"palette start ({bx}, {by}) agrees for {agree:.2f} of {n} matched tiles; "
                           f"need {MIN_AGREE}")
    for pal, depth in _used(layers, screens, info):
        width = 16 if depth == 4 else 256
        x, y = bx + (pal % 16) * 16, by + pal // 16
        if x < 0 or y < 0 or x + width > VRAM_W or y >= VRAM_H:
            raise PaletteError(f"palette {pal} ({depth} bpp) would be at VRAM ({x}, {y}), outside VRAM")
    return {"clut_x": bx, "clut_y": by, "matches": n, "agree": round(agree, 3),
            "cameras": [list(c) if c is not None else None for c in cams]}


def palette(vram, rule, n, count):
    o = (rule["clut_y"] + n // 16) * ROW + (rule["clut_x"] + (n % 16) * 16) * 2
    return [to_rgba(c) for c in struct.unpack_from(f"<{count}H", vram, o)]
