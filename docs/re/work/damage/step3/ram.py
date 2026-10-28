import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path.home() / "IdeaProjects/mmx-demake-tools"))
from mmxdemake.savestate import read_ram
from mmxdemake.mips import disassemble

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"


def load(name):
    p = Path(name)
    if not p.exists():
        p = DUMPS / name
    return read_ram(p, EXE.read_bytes())


def word(ram, a):
    return struct.unpack_from("<I", ram, a & 0x1FFFFF)[0]


def dis(ram, start, end):
    for a in range(start, end, 4):
        w = word(ram, a)
        print(f"{a:08x}: {w:08x}  {disassemble(w, a)}")


def scan_store(ram, lo, hi, offset, ops=(0x28, 0x29, 0x2B)):
    for a in range(lo, hi, 4):
        w = word(ram, a)
        if (w >> 26) in ops and (w & 0xFFFF) == offset:
            print(f"{a:08x}: {disassemble(w, a)}")


def scan_imm(ram, lo, hi, value):
    for a in range(lo, hi, 4):
        w = word(ram, a)
        op = w >> 26
        if op in (0x09, 0x0D, 0x0F, 0x0C, 0x0A, 0x0B) and (w & 0xFFFF) == value:
            print(f"{a:08x}: {disassemble(w, a)}")


if __name__ == "__main__":
    cmd, state = sys.argv[1], sys.argv[2]
    ram = load(state)
    args = [int(x, 0) for x in sys.argv[3:]]
    {"dis": dis, "store": scan_store, "imm": scan_imm}[cmd](ram, *args)
