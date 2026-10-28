import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))

from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad

DUMPS = Path.home() / "mmx-data/x6/dumps"
EXE = (Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95").read_bytes()


def load(name):
    if not name.endswith(".state.gz"):
        name += ".state.gz"
    st = read_state(DUMPS / name)
    return ram_from(st, EXE, name), scratchpad(st)


def u8(ram, a):
    return ram[a & 0x1FFFFF]


def s8(ram, a):
    v = u8(ram, a)
    return v - 256 if v > 127 else v


def u16(ram, a):
    return struct.unpack_from("<H", ram, a & 0x1FFFFF)[0]


def s16(ram, a):
    return struct.unpack_from("<h", ram, a & 0x1FFFFF)[0]


def u32(ram, a):
    return struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]


def s32(ram, a):
    return struct.unpack_from("<i", ram, a & 0x1FFFFF)[0]


def sp32(scr, off):
    return struct.unpack_from("<I", scr, off)[0]


def hexd(ram, a, n):
    return ram[a & 0x1FFFFF:(a & 0x1FFFFF) + n].hex(" ")
