import struct

from mmxdemake.problem import Problem

SIDE = 16
SCREEN_BYTES = 0x200
CELLS = SIDE * SIDE
MAX_LISTED = 20


def decode_screens(data):
    if len(data) % SCREEN_BYTES:
        raise ValueError(f"screens entry is {len(data)} bytes, not a multiple of {SCREEN_BYTES}")
    values = struct.unpack(f"<{len(data) // 2}H", data)
    return [list(values[i:i + CELLS]) for i in range(0, len(values), CELLS)]


def decode_tile_info(data):
    if len(data) % 4:
        raise ValueError(f"tile info entry is {len(data)} bytes, not a multiple of 4")
    return list(struct.unpack(f"<{len(data) // 4}I", data))


def tile(v):
    return v & 0x3FFF


def flags(v):
    return v >> 14


def collision(w):
    return w & 0x3F


def check(screens, info):
    bad = [(s, n % SIDE, n // SIDE, tile(v))
           for s, screen in enumerate(screens) for n, v in enumerate(screen) if tile(v) >= len(info)]
    problems = [Problem(f"screen {s} cell ({x},{y})", f"tile {t} is not below the tile count {len(info)}")
                for s, x, y, t in bad[:MAX_LISTED]]
    if len(bad) > MAX_LISTED:
        problems.append(Problem("screens", f"{len(bad) - MAX_LISTED} more cells out of range"))
    return problems
