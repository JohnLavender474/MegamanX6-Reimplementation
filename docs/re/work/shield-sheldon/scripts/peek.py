import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram
EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"

def load(name="sheldon-17_boss.state.gz"):
    return read_ram(DUMPS / name, EXE.read_bytes())

class R:
    def __init__(self, ram): self.ram = ram
    def b(self, a): return self.ram[a & 0x1FFFFF]
    def sb(self, a): v = self.b(a); return v - 256 if v > 127 else v
    def h(self, a): return struct.unpack_from("<H", self.ram, a & 0x1FFFFF)[0]
    def sh(self, a): return struct.unpack_from("<h", self.ram, a & 0x1FFFFF)[0]
    def w(self, a): return struct.unpack_from("<I", self.ram, a & 0x1FFFFF)[0]
    def sw(self, a): return struct.unpack_from("<i", self.ram, a & 0x1FFFFF)[0]
    def bytes(self, a, n): return self.ram[a & 0x1FFFFF:(a & 0x1FFFFF) + n]

def dmg_table(r, a):
    out = []
    for i in range(80):
        c, d = r.b(a + 2 * i), r.b(a + 2 * i + 1)
        out.append("D" if c == 0xFF else "–" if c == 2 else str(d) if c == 0 else f"c{c}")
    return out
