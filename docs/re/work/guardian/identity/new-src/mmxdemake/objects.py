import re
import struct

from mmxdemake.savestate import ram_at

LIST_POINTER = 0x8007349C
TABLE_STRIDE = 8
TABLE_ENTRIES = 23
AREA_STRIDE = 4
AREAS = 2
GAME_AREA = 0x800CCEDD
RECORD = 8
END_KIND = 0x0F
MAX_RECORDS = 1024
SCREEN_PX = 256
RAM_BASE = 0x80000000
RAM_END = 0x80200000
FORMAT = "<BBBBHH"
FIELDS = ("a", "b", "id", "kind", "x", "y")


class ObjectError(Exception):
    pass


def list_at(buf, start):
    for n in range(MAX_RECORDS + 1):
        k = start + n * RECORD
        if k + RECORD > len(buf):
            raise ObjectError(f"object list at {start:#x} runs to the end of the data without an end record")
        if buf[k + 3] == END_KIND:
            return bytes(buf[start:k + RECORD])
    raise ObjectError(f"object list at {start:#x} has no end record in {MAX_RECORDS} records")


def read_list(ram, entry=0, area=0):
    addr = LIST_POINTER + entry * TABLE_STRIDE + area * AREA_STRIDE
    (p,) = struct.unpack("<I", ram_at(ram, addr, 4))
    if not RAM_BASE <= p < RAM_END or p % 2:
        raise ObjectError(f"object list pointer {p:#x} at {addr:#x} is not a RAM address")
    return list_at(ram, p - RAM_BASE)


def find_stage_list(ram, w, h, overlays, layout_overlay):
    found = []
    for entry in range(TABLE_ENTRIES):
        for area in range(AREAS):
            try:
                data = read_list(ram, entry, area)
                decode(data, w, h)
                overlay, offset = find_list(data, overlays)
            except ObjectError:
                continue
            if overlay == layout_overlay:
                found.append((entry, area, overlay, offset, data))
    if len({f[1] for f in found}) > 1:
        current = ram_at(ram, GAME_AREA, 1)[0]
        found = [f for f in found if f[1] == current]
    if len(found) != 1:
        raise ObjectError(f"{len(found)} object table entries ({[f[0] for f in found]}) give a list in overlay "
                          f"{layout_overlay}; need exactly 1")
    return found[0]


def _kind_class(kind):
    return b"[" + b"".join(re.escape(bytes([high << 4 | kind & 0x0F])) for high in range(16)) + b"]"


def find_list(data, overlays):
    pattern = re.compile(b"".join(b"." + re.escape(data[k + 1:k + 3]) + _kind_class(data[k + 3])
                                  + re.escape(data[k + 4:k + RECORD]) for k in range(0, len(data), RECORD)),
                         re.DOTALL)
    hits = []
    for i, body in sorted(overlays.items()):
        k = 0
        while (m := pattern.search(body, k)) is not None:
            hits.append((i, m.start()))
            k = m.start() + 1
    if len(hits) != 1:
        raise ObjectError(f"object list found {len(hits)} times in the overlays; need exactly 1")
    i, k = hits[0]
    before = overlays[i][max(k - RECORD, 0):k]
    if k >= RECORD and any(before) and before[3] != END_KIND:
        raise ObjectError(f"object list at overlay {i} {k:#x} follows another record; "
                          f"the pointer may not be the list start")
    return hits[0]


def decode(data, w, h):
    if len(data) % RECORD:
        raise ObjectError(f"object list is {len(data)} bytes, not a multiple of {RECORD}")
    records = []
    for index in range(len(data) // RECORD):
        r = dict(zip(FIELDS, struct.unpack_from(FORMAT, data, index * RECORD)), index=index)
        if r["kind"] == END_KIND:
            return records
        if r["x"] >= w * SCREEN_PX or r["y"] >= h * SCREEN_PX:
            raise ObjectError(f"object {index} at ({r['x']}, {r['y']}) is outside the {w}x{h} stage")
        records.append(r)
    raise ObjectError("object list has no end record")
