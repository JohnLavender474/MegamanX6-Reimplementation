import struct
from collections import namedtuple

from mmxdemake.mips import written
from mmxdemake.savestate import ram_at

OVERLAY_BASE = 0x800E9860
BOSS_OVERLAY_BASE = 0x800FA000
PLAYER_OVERLAY_BASE = 0x801EA000
OVERLAY_PROBE = 0x800
PROGRAM_BASE = 0x80010000
EXE_HEADER = 0x800
MAIN_TABLE = 0x80
MAIN_TYPES = 77
ANIM_FIELD = 0x30
SPRITE_FIELD = 0x3C
TEXTURE_FIELD = 0x40
MODE_BITS = 0x60
LOOKBACK = 12
PAIR_GAP = 5
RUN_MAX = 64
SLOTS = 48
SLOT_BASE = 0x8008EF48
SLOT_SIZE = 0x9C
JR_RA = 0x03E00008
STACK_ADJUST = 0x27BD
OP_J = 2
OP_JAL = 3
OP_ADDIU = 9
OP_LUI = 15
OP_SW = 0x2B


ObjectClass = namedtuple("ObjectClass", "name base slots size types")
MAIN_OBJECTS = ObjectClass("main", SLOT_BASE, SLOTS, SLOT_SIZE, MAIN_TYPES)
STAGE_OBJECTS = ObjectClass("stage", 0x800C1220, 80, 0xA4, 32)


class FindError(Exception):
    pass


def _u32(ram, addr):
    return struct.unpack("<I", ram_at(ram, addr, 4))[0]


def _s16(x):
    return (x & 0xFFFF) - ((x & 0x8000) << 1)


def _rs(x):
    return (x >> 21) & 31


def _rt(x):
    return (x >> 16) & 31


def _hexes(values):
    return ", ".join(f"{v:#x}" for v in values)


def _matches(ram, overlays, base):
    probe = ram_at(ram, base, OVERLAY_PROBE)
    return [n for n, body in sorted(overlays.items()) if body[:OVERLAY_PROBE] == probe]


def find_overlay(ram, overlays, base=OVERLAY_BASE):
    matches = _matches(ram, overlays, base)
    if len(matches) != 1:
        raise FindError(f"{len(matches)} overlays match RAM at {base:#x} ({matches}); need exactly 1")
    n = matches[0]
    return n, base, base + len(overlays[n])


def player_overlay(ram, overlays):
    return find_overlay(ram, overlays, PLAYER_OVERLAY_BASE)


def boss_overlay(ram, overlays):
    matches = _matches(ram, overlays, BOSS_OVERLAY_BASE)
    if len(matches) > 1:
        raise FindError(f"{len(matches)} overlays match RAM at {BOSS_OVERLAY_BASE:#x} ({matches}); need at most 1")
    if not matches:
        return None
    n = matches[0]
    return n, BOSS_OVERLAY_BASE, BOSS_OVERLAY_BASE + len(overlays[n])


def program_bounds(exe):
    return PROGRAM_BASE, PROGRAM_BASE + len(exe) - EXE_HEADER


def code_regions(ram, overlays, exe):
    _, lo, hi = find_overlay(ram, overlays)
    boss = boss_overlay(ram, overlays)
    return [(lo, hi), program_bounds(exe)] + ([boss[1:]] if boss else [])


def stage_types(ram, scratch, lo, hi):
    table = struct.unpack_from("<I", scratch, MAIN_TABLE)[0]
    out = {}
    for t in range(MAIN_TYPES):
        func = _u32(ram, table + 4 * t)
        if lo <= func < hi:
            out[t] = func
    return out


def _live(ram, regions, cls):
    for s in range(cls.slots):
        a = cls.base + s * cls.size
        head = ram_at(ram, a, 2)
        table = _u32(ram, a + ANIM_FIELD)
        if head[0] and head[1] < cls.types and any(lo <= table < hi for lo, hi in regions):
            yield a, head, table


def _source(ram, a, head):
    data = _u32(ram, a + SPRITE_FIELD)
    texture, clut = struct.unpack("<HH", ram_at(ram, a + TEXTURE_FIELD, 4))
    return data, texture, clut, head[0] & MODE_BITS


def live_tables(ram, regions, cls=MAIN_OBJECTS):
    out = {}
    for _, head, table in _live(ram, regions, cls):
        out.setdefault(head[1], set()).add(table)
    return out


def live_sources(ram, regions, cls=MAIN_OBJECTS):
    out = {}
    for a, head, _ in _live(ram, regions, cls):
        source = _source(ram, a, head)
        if source[0]:
            out.setdefault(head[1], set()).add(source)
    return out


def live_objects(ram, regions, cls):
    found = set()
    for a, head, table in _live(ram, regions, cls):
        source = _source(ram, a, head)
        if source[0]:
            found.add((head[1], table) + source)
    return sorted(found)


def _last_write(ram, pc, reg, lo, limit):
    for k in range(1, limit + 1):
        a = pc - 4 * k
        if a < lo:
            break
        y = _u32(ram, a)
        if written(y) == reg:
            return a, y
    return None, None


def _constant(ram, pc, reg, lo):
    a, y = _last_write(ram, pc, reg, lo, LOOKBACK)
    if y is None or y >> 26 != OP_ADDIU:
        return None
    _, z = _last_write(ram, a, _rs(y), lo, LOOKBACK - (pc - a) // 4)
    if z is None or z >> 26 != OP_LUI:
        return None
    return (((z & 0xFFFF) << 16) + _s16(y)) & 0xFFFFFFFF


def stores(ram, lo, hi):
    out = []
    for pc in range(lo, hi, 4):
        x = _u32(ram, pc)
        if x >> 26 != OP_SW or x & 0xFFFF != ANIM_FIELD:
            continue
        table = _constant(ram, pc, _rt(x), lo)
        if table is not None and lo <= table < hi:
            out.append((pc, table))
    return out


def function_start(ram, pc, lo):
    a = pc & ~3
    while a > lo:
        x = _u32(ram, a)
        if x >> 16 == STACK_ADJUST and x & 0x8000:
            return a
        if _u32(ram, a - 8) == JR_RA:
            return a
        a -= 4
    return lo


def references(ram, lo, hi):
    refs = {}
    for o in range(lo, hi, 4):
        x = _u32(ram, o)
        op = x >> 26
        if op in (OP_J, OP_JAL):
            target = (o & 0xF0000000) | ((x & 0x3FFFFFF) << 2)
            refs.setdefault(target, set()).add(function_start(ram, o, lo))
        elif lo <= x < hi:
            refs.setdefault(x, set()).add(("data", o))
        elif op == OP_LUI:
            for k in range(1, PAIR_GAP + 1):
                y = _u32(ram, o + 4 * k)
                if y >> 26 == OP_ADDIU and _rs(y) == _rt(x):
                    value = (((x & 0xFFFF) << 16) + _s16(y)) & 0xFFFFFFFF
                    if lo <= value < hi:
                        refs.setdefault(value, set()).add(function_start(ram, o, lo))
                    break
    return refs


def _run_callers(ram, addr, refs, lo, hi):
    out, a = [], addr
    for _ in range(RUN_MAX):
        out.extend(r for r in refs.get(a, ()) if not isinstance(r, tuple))
        if not lo <= _u32(ram, a - 4) < hi:
            break
        a -= 4
    return out


def owners(ram, func, refs, entries, lo, hi):
    found, seen, todo = set(), set(), [func]
    while todo:
        f = todo.pop()
        if f in seen:
            continue
        seen.add(f)
        if f in entries:
            found |= entries[f]
            continue
        for r in refs.get(f, ()):
            todo.extend(_run_callers(ram, r[1], refs, lo, hi) if isinstance(r, tuple) else [r])
    return found


def searched_tables(ram, scratch, lo, hi):
    entries = {}
    for t, func in stage_types(ram, scratch, lo, hi).items():
        entries.setdefault(func, set()).add(t)
    refs = references(ram, lo, hi)
    out = {}
    for pc, table in stores(ram, lo, hi):
        for t in owners(ram, function_start(ram, pc, lo), refs, entries, lo, hi):
            out.setdefault(t, set()).add(table)
    return {t: sorted(v) for t, v in sorted(out.items())}


def choose(catalog_table, live, searched):
    if catalog_table is not None:
        return [(catalog_table, "catalog")], None
    live = set(live or ())
    tables = sorted(live | set(searched or ()))
    if not tables:
        return [], "no live object and no table found by the search"
    note = f"{len(tables)} tables: {_hexes(tables)}; each is exported" if len(tables) > 1 else None
    return [(t, "live" if t in live else "search") for t in tables], note
