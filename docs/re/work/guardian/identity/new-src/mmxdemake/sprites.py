import struct
from collections import namedtuple

from mmxdemake.psxcolor import to_rgba
from mmxdemake.savestate import RAM_OFFSET, RAM_SIZE, ram_at

STAGE = 0x800CCEDC
AREA = 0x800CCEDD
TYPE_LISTS = 0x800733C4
TEXTURE_TABLE = 0x80095098
BANK_BASE = 0x20
SCRATCHPAD_OFFSET = RAM_OFFSET + RAM_SIZE + 0x80000
SCRATCHPAD_SIZE = 0x400
LIST_END = 0xFF
MAX_TYPES = 255
MAX_TEXTURES = 64
CLUT_Y0 = 482
ENEMY_MODE = 0x40
CELL = 16
CELL_BYTES = 8
ROW = 2048
VRAM_H = 512
PAGES = 32
WIDE = 0x20
MIRROR = 0x40
FLIP = 0x80
CLEAR = (0, 0, 0, 0)

Bank = namedtuple("Bank", "index type data count texture clut")
Image = namedtuple("Image", "width height ox oy pixels")


class SpriteError(Exception):
    pass


def _u8(ram, addr):
    return ram_at(ram, addr, 1)[0]


def _u16(ram, addr):
    return struct.unpack("<H", ram_at(ram, addr, 2))[0]


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def scratchpad(state):
    data = state[SCRATCHPAD_OFFSET:SCRATCHPAD_OFFSET + SCRATCHPAD_SIZE]
    if len(data) != SCRATCHPAD_SIZE:
        raise SpriteError(f"save state is {len(state)} bytes, too short for the scratchpad at {SCRATCHPAD_OFFSET:#x}")
    return data


def stage_area(ram):
    return _u8(ram, STAGE), _u8(ram, AREA)


def type_list(ram):
    stage, area = stage_area(ram)
    ptr = _u32(ram, TYPE_LISTS + 4 * (stage * 2 + area))
    if not ptr:
        raise SpriteError(f"stage {stage} area {area} has no type list")
    types = []
    while len(types) < MAX_TYPES:
        t = _u8(ram, ptr + len(types))
        if t == LIST_END:
            return types
        types.append(t)
    raise SpriteError(f"type list at {ptr:#x} has no end marker")


def clut_word(i):
    return ((CLUT_Y0 + i // 4) << 6) | (4 * (i % 4))


def banks(ram, scratch):
    base = struct.unpack_from("<I", scratch, BANK_BASE)[0]
    table = _u32(ram, TEXTURE_TABLE)
    if base < table or (base - table) % 4 or (base - table) // 4 > MAX_TEXTURES:
        raise SpriteError(f"texture table {table:#x} and sprite bank base {base:#x} do not agree")
    textures = (base - table) // 4
    entries = _u32(ram, base) // 4
    out = []
    for i, t in enumerate(type_list(ram)[:entries]):
        data = base + _u32(ram, base + 4 * i)
        texture = _u32(ram, table + 4 * i) >> 7 if i < textures else None
        out.append(Bank(i, t, data, _u16(ram, data + 2), texture, clut_word(i)))
    return out


def entries(ram, scratch):
    base = struct.unpack_from("<I", scratch, BANK_BASE)[0]
    return [base + _u32(ram, base + 4 * i) for i in range(_u32(ram, base) // 4)]


def bank_of_type(bank_list, t):
    return next((b for b in bank_list if b.type == t), None)


def pieces(ram, bank, n):
    if not 0 <= n < bank.count:
        raise SpriteError(f"bank {bank.index}: sprite {n} is out of range (count {bank.count})")
    count = _u16(ram, bank.data + 4 * n)
    offset = _u16(ram, bank.data + 4 * n + 2)
    return [struct.unpack("<BBbb", ram_at(ram, bank.data + 4 * (offset + j), 4)) for j in range(count)]


def remap(t, mode):
    if mode == 0x60:
        return ((t >> 4) << 8) + (t & 15) + 0x600
    if mode == 0x40:
        q = t // 160
        return (q << 8) + t - 160 * q + 0x610
    if mode == 0x20:
        q = t // 80
        return (q << 8) + t - 80 * q + 0x5B0
    return t


def cell(flags, tile, texture, mode):
    if flags & WIDE:
        t = ((tile | (flags & 1) << 8) + texture) & 0xFFFF
    else:
        t = remap(((tile | (flags & 3) << 8) + texture) & 0xFFFF, mode) & 0xFFFF
    if t >> 8 >= PAGES:
        raise SpriteError(f"texture cell {t:#x} is outside the 4 bpp pages")
    return t >> 8, (t & 15) * CELL, ((t >> 4) & 15) * CELL


def cell_bytes(vram, page, u, v):
    x = (page & 15) * 64 + u // 4
    y = (page >> 4) * 256 + v
    return b"".join(vram[(y + r) * ROW + x * 2:(y + r) * ROW + x * 2 + CELL_BYTES] for r in range(CELL))


def palette_words(vram, clut):
    x, y = (clut & 63) * 16, clut >> 6
    if y >= VRAM_H:
        raise SpriteError(f"palette {clut:#x} is outside VRAM")
    return list(struct.unpack_from("<16H", vram, y * ROW + x * 2))


def loaded(bank):
    if bank.texture is None:
        raise SpriteError(f"bank {bank.index} (type {bank.type}) has no texture in VRAM")
    return bank.texture


def with_source(ram, bank, data, texture, clut):
    return bank._replace(data=data, count=_u16(ram, data + 2), texture=texture, clut=clut)


def cells(ram, bank, mode=ENEMY_MODE):
    texture = loaded(bank)
    out = set()
    for n in range(bank.count):
        for flags, tile, _, _ in pieces(ram, bank, n):
            try:
                out.add(cell(flags, tile, texture, mode))
            except SpriteError:
                continue
    return out


def _texel(data, x, y):
    return (data[y * CELL_BYTES + x // 2] >> (4 * (x & 1))) & 15


def draw(ram, vram, bank, n, mode=ENEMY_MODE):
    texture = loaded(bank)
    canvas = {}
    for flags, tile, dx, dy in pieces(ram, bank, n):
        page, u, v = cell(flags, tile, texture, mode)
        data = cell_bytes(vram, page, u, v)
        colours = palette_words(vram, bank.clut + ((flags >> 2) & 3))
        wide = flags & WIDE
        x0 = 2 * dx + (1 if flags & FLIP else 0) if wide else dx
        for yy in range(CELL):
            sy = 15 - yy if flags & FLIP and not wide else yy
            for xx in range(CELL):
                sx = 15 - xx if flags & MIRROR and not wide else xx
                c = colours[_texel(data, sx, sy)]
                if c:
                    canvas[(x0 + xx, dy + yy)] = to_rgba(c)
    if not canvas:
        return Image(1, 1, 0, 0, [[CLEAR]])
    left = min(x for x, _ in canvas)
    top = min(y for _, y in canvas)
    width = max(x for x, _ in canvas) - left + 1
    height = max(y for _, y in canvas) - top + 1
    pixels = [[canvas.get((left + x, top + y), CLEAR) for x in range(width)] for y in range(height)]
    return Image(width, height, -left, -top, pixels)
