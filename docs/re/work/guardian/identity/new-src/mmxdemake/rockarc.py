import struct
from dataclasses import dataclass
from math import ceil

from mmxdemake.problem import Problem

SECTOR = 2048
MAX_SUBENTRIES = 64


@dataclass(frozen=True)
class Member:
    index: int
    sector: int
    size: int


def parse_toc(header):
    members = []
    for i in range(len(header) // 8):
        sector, size = struct.unpack_from("<II", header, i * 8)
        if sector == 0 and size == 0:
            break
        members.append(Member(i, sector, size))
    return members


def check_toc(name, members, header, file_sectors, header_sectors=2, expected_count=None):
    where = f"{name} TOC"
    problems = []
    terminator = len(members) * 8
    if terminator + 8 > len(header):
        problems.append(Problem(where, "no zero pair ends the list"))
    else:
        tail = header[terminator + 8:]
        nonzero = next((i for i, b in enumerate(tail) if b), None)
        if nonzero is not None:
            problems.append(Problem(where, f"header byte {terminator + 8 + nonzero} after the zero pair is not zero"))
    if expected_count is not None and len(members) != expected_count:
        problems.append(Problem(where, f"{len(members)} members, expected {expected_count}"))
    if not members:
        problems.append(Problem(where, "no members"))
        return problems
    if members[0].sector != header_sectors:
        problems.append(Problem(where, f"member 0 starts at sector {members[0].sector}, expected {header_sectors}"))
    for a, b in zip(members, members[1:]):
        expected = a.sector + ceil(a.size / SECTOR)
        if b.sector != expected:
            problems.append(Problem(where, f"member {b.index} starts at sector {b.sector}, expected {expected}"))
    last = members[-1]
    end = last.sector + ceil(last.size / SECTOR)
    if end != file_sectors:
        problems.append(Problem(where, f"last member ends at sector {end}, file has {file_sectors} sectors"))
    for m in members:
        if m.size < 4:
            problems.append(Problem(where, f"member {m.index} size {m.size} is below 4"))
    return problems


def read_member(data, m):
    start = m.sector * SECTOR
    if start + m.size > len(data):
        raise ValueError(f"member {m.index} runs past the end of the archive")
    return data[start:start + m.size]


def tail_padding_is_zero(data, m):
    start = m.sector * SECTOR + m.size
    end = (m.sector + ceil(m.size / SECTOR)) * SECTOR
    return not any(data[start:end])


def observe_header(body, tail_zero):
    observed = {"tail_zero": tail_zero}
    if len(body) < 8:
        return observed
    word0, word1 = struct.unpack_from("<II", body)
    observed["word0"], observed["word1"] = word0, word1
    if 0 < word0 < MAX_SUBENTRIES and word1 == len(body) and 8 + 8 * word0 <= len(body):
        observed["subentries"] = [list(struct.unpack_from("<II", body, 8 + 8 * k)) for k in range(word0)]
    return observed
