import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
MAIN, VIS = 0x8008EF48, 0x80093C98


class R:
    def __init__(self, ram): self.ram = ram
    def b(self, a): return self.ram[a & 0x1FFFFF]
    def sb(self, a): v = self.b(a); return v - 256 if v > 127 else v
    def h(self, a): return struct.unpack_from("<H", self.ram, a & 0x1FFFFF)[0]
    def sh(self, a): return struct.unpack_from("<h", self.ram, a & 0x1FFFFF)[0]
    def w(self, a): return struct.unpack_from("<I", self.ram, a & 0x1FFFFF)[0]
    def sw(self, a): return struct.unpack_from("<i", self.ram, a & 0x1FFFFF)[0]


def obj(r, a):
    return (f"{a:08x} flags {r.b(a):02x} type {r.b(a+1)} var {r.b(a+2)} disp {r.b(a+3)} "
            f"state {r.b(a+4)} step {r.b(a+5)} step2 {r.b(a+6)} act4 {r.b(a+7)} "
            f"pos ({r.sh(a+0xA)}, {r.sh(a+0xE)}) flip {r.b(a+0x15):02x} prio {r.b(a+0x16)} "
            f"anime {r.b(a+0x17)} next {r.sb(a+0x46)} spd ({r.sw(a+0x20)/65536:.3f}, {r.sw(a+0x24)/65536:.3f}) "
            f"grav {r.sw(a+0x2C)/65536:.3f} tbl {r.w(a+0x30):08x} atk {r.w(a+0x50):08x} hurt {r.w(a+0x54):08x} "
            f"dmg {r.w(a+0x58):08x} hp {r.sb(a+0x5C)} cd {r.b(a+0x60)} terr {r.w(a+0x68):08x} "
            f"coll {r.b(a+0x70):02x} 7C {r.h(a+0x7C)} 80 {r.w(a+0x80):08x} rec {r.w(a+0x10):08x}")


def main(state):
    r = R(read_ram(DUMPS / state, EXE.read_bytes()))
    m = 0x800970A0
    print(state, "player x", r.sh(m + 0xA), "y", r.sh(m + 0xE), "chara", r.b(m + 2),
          "step", r.b(m + 5), "game.stage", r.b(0x800CCED0 + 0x0), "")
    for i in range(48):
        a = MAIN + i * 0x9C
        if r.b(a) and r.b(a + 1) == 6:
            print("main", i, obj(r, a))
            rec = r.w(a + 0x10)
            if rec:
                print("   record", r.ram[rec & 0x1FFFFF:(rec & 0x1FFFFF) + 8].hex(" "))
    for i in range(32):
        a = VIS + i * 0xA0
        if r.b(a) and r.b(a + 1) == 5:
            print("visual", i, obj(r, a), "parent", f"{r.w(a+0x7C):08x}")


if __name__ == "__main__":
    for s in sys.argv[1:] or ["wolfang-2_ice-slope.state.gz"]:
        main(s)
