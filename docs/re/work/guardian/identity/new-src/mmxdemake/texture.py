from dataclasses import dataclass

BLOCK_SIZE = 0x40000
PAGE_4BPP = 0x8000
PAGE_8BPP = 0x10000
HALF_8BPP = 0x8000
ROW = 128
TILE = 16


@dataclass(frozen=True)
class TileRef:
    page: int
    cx: int
    cy: int
    palette: int


def tile_ref(w):
    return TileRef((w >> 24) & 0xF, (w >> 16) & 0xF, (w >> 20) & 0xF, (w >> 8) & 0xFF)


def bits(page):
    if page < 8:
        return 4
    if page <= 11:
        return 8
    return None


def raw_tile(block, ref):
    if len(block) != BLOCK_SIZE:
        raise ValueError(f"texture block is {len(block)} bytes, need {BLOCK_SIZE}")
    kind = bits(ref.page)
    if kind is None:
        return None
    values = []
    for r in range(TILE):
        y = ref.cy * TILE + r
        if kind == 4:
            start = ref.page * PAGE_4BPP + y * ROW + ref.cx * 8
            for b in block[start:start + 8]:
                values += (b & 15, b >> 4)
        else:
            base = (ref.page & 3) * PAGE_8BPP + (HALF_8BPP if ref.cx >= 8 else 0)
            start = base + y * ROW + (ref.cx % 8) * TILE
            values += block[start:start + TILE]
    return values


def covered_4bpp_cells(ref):
    if bits(ref.page) != 8:
        return set()
    page4 = 2 * (ref.page & 3) + (1 if ref.cx >= 8 else 0)
    x4 = 2 * (ref.cx % 8)
    return {(page4, x4, ref.cy), (page4, x4 + 1, ref.cy)}
