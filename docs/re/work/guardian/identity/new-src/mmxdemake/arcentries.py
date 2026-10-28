import struct
from dataclasses import dataclass
from math import ceil

HEADER = 0x800
MAX_COUNT = 64


@dataclass(frozen=True)
class Entry:
    type: int
    offset: int
    size: int


class ArcError(Exception):
    pass


def split(body):
    if len(body) < HEADER:
        raise ArcError(f"member is {len(body)} bytes, below the 0x800 header")
    count, size = struct.unpack_from("<II", body)
    if not 0 < count < MAX_COUNT:
        raise ArcError(f"entry count {count} is not in 1..63")
    if size != len(body):
        raise ArcError(f"header size {size} != member size {len(body)}")
    entries, offset = [], HEADER
    for k in range(count):
        entry_type, entry_size = struct.unpack_from("<II", body, 8 + 8 * k)
        if offset + entry_size > len(body):
            raise ArcError(f"entry {k} (type {entry_type:#x}) at {offset} size {entry_size} "
                           f"runs past the member end {len(body)}")
        entries.append(Entry(entry_type, offset, entry_size))
        offset += ceil(entry_size / HEADER) * HEADER
    if offset != len(body):
        raise ArcError(f"entries end at {offset}, member size is {len(body)}")
    return entries


def entry_data(body, e):
    return body[e.offset:e.offset + e.size]


def only(entries, type_):
    found = [e for e in entries if e.type == type_]
    if len(found) != 1:
        raise ArcError(f"{len(found)} entries of type {type_:#x}, need exactly 1")
    return found[0]
