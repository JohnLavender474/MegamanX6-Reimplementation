import struct
from dataclasses import dataclass
from math import ceil

from mmxdemake.cdimage import FORM1_DATA, DiscError

PVD_LBA = 16
XA_FORM2 = 0x1000
FLAG_DIRECTORY = 0x02


@dataclass(frozen=True)
class IsoFile:
    path: str
    lba: int
    size: int
    form: int
    first_submode: int


def _u32(b, off):
    return struct.unpack_from("<I", b, off)[0]


def _read_extent(disc, lba, length):
    return b"".join(disc.form1(lba + i) for i in range(ceil(length / FORM1_DATA)))


def _xa_attributes(record, name_len):
    start = 33 + name_len + (1 if name_len % 2 == 0 else 0)
    su = record[start:]
    if len(su) >= 14 and su[6:8] == b"XA":
        return int.from_bytes(su[4:6], "big")
    return None


def walk(disc):
    pvd = disc.form1(PVD_LBA)
    if pvd[0] != 1 or pvd[1:6] != b"CD001":
        raise DiscError(f"LBA {PVD_LBA}: no ISO9660 primary volume descriptor")
    root = pvd[156:190]
    out = []
    _walk_dir(disc, _u32(root, 2), _u32(root, 10), "", out, set())
    return sorted(out, key=lambda f: f.path)


def _walk_dir(disc, lba, length, prefix, out, seen):
    if lba in seen:
        raise DiscError(f"directory loop at LBA {lba} ({prefix or '/'})")
    seen.add(lba)
    data = _read_extent(disc, lba, length)
    pos = 0
    while pos < length:
        n = data[pos]
        if n == 0:
            pos = (pos // FORM1_DATA + 1) * FORM1_DATA
            continue
        record = data[pos:pos + n]
        pos += n
        name_len = record[32]
        raw_name = record[33:33 + name_len]
        if raw_name in (b"\x00", b"\x01"):
            continue
        name = raw_name.decode("ascii").split(";")[0]
        entry_lba, entry_size, flags = _u32(record, 2), _u32(record, 10), record[25]
        if flags & FLAG_DIRECTORY:
            _walk_dir(disc, entry_lba, entry_size, prefix + name + "/", out, seen)
            continue
        attr = _xa_attributes(record, name_len)
        if attr is None:
            raise DiscError(f"{prefix}{name}: directory record has no XA attribute")
        form = 2 if attr & XA_FORM2 else 1
        out.append(IsoFile(prefix + name, entry_lba, entry_size, form, disc.submode(entry_lba)))


def iter_file(disc, f):
    count = ceil(f.size / FORM1_DATA)
    if f.form == 2:
        for k in range(count):
            yield disc.form2_raw(f.lba + k)
        return
    remaining = f.size
    for k in range(count):
        yield disc.form1(f.lba + k)[:min(FORM1_DATA, remaining)]
        remaining -= FORM1_DATA


def read_file(disc, f):
    return b"".join(iter_file(disc, f))
