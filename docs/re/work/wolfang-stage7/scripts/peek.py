"""Print the live stage object 7 slots, the type-6 main objects, the misc type 9
objects, the stage logic 8 object and the camera limits of a save state.
Usage (repo root, PYTHONPATH=.): peek.py <state.gz> ..."""
import struct, sys
from pathlib import Path
from mmxdemake.savestate import read_ram

EXE = Path.home() / "mmx-data/x6/extracted/retail/iso/SLUS_013.95"
DUMPS = Path.home() / "mmx-data/x6/dumps"
MAIN, STAGE, MEGA, GAME, CAM = 0x8008EF48, 0x800C1220, 0x800970A0, 0x800CCED0, 0x800971F8


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
            f"state {r.b(a+4)} step {r.b(a+5)} step2 {r.b(a+6)} act4 {r.sb(a+7)} "
            f"pos ({r.sh(a+0xA)}, {r.sh(a+0xE)}) sub ({r.h(a+0x8)}, {r.h(a+0xC)}) prev_y {r.sh(a+0x1E)} "
            f"prio {r.b(a+0x16)} anime {r.b(a+0x17)} spd ({r.sw(a+0x20)/65536:.3f}, {r.sw(a+0x24)/65536:.3f}) "
            f"grav {r.sw(a+0x2C)/65536:.3f} tbl {r.w(a+0x30):08x} sprite {r.b(a+0x47)} hurt {r.w(a+0x54):08x} "
            f"dmg {r.w(a+0x58):08x} hp {r.sb(a+0x5C)} cd {r.b(a+0x60)} terr {r.w(a+0x68):08x} "
            f"coll {r.b(a+0x70):02x} 72 {r.b(a+0x72):02x} 76 {r.b(a+0x76):02x} 7C {r.w(a+0x7C):08x} "
            f"80 {r.ram[(a+0x80)&0x1FFFFF:(a+0x84)&0x1FFFFF].hex()} 84 {r.b(a+0x84)} "
            f"86..8B {list(r.ram[(a+0x86)&0x1FFFFF:(a+0x8C)&0x1FFFFF])} A2 {r.b(a+0xA2)} rec {r.w(a+0x10):08x}")


def main(state):
    r = R(read_ram(DUMPS / state, EXE.read_bytes()))
    print(state)
    print(f"  player x {r.sh(MEGA+0xA)} y {r.sh(MEGA+0xE)} chara {r.b(MEGA+2)} state {r.b(MEGA+4)} "
          f"step {r.b(MEGA+5)} hp {r.b(MEGA+0x5C)} E0 {r.b(MEGA+0xE0)} E5 {r.b(MEGA+0xE5)} E6 {r.b(MEGA+0xE6)} "
          f"71 {r.b(MEGA+0x71):02x} 72 {r.b(MEGA+0x72):02x}")
    print(f"  game stage {r.b(GAME)} area {r.b(GAME+1)} difficulty {r.b(0x800CCF38)} frame?")
    print(f"  camera0 x {r.sh(CAM+0x20)} y {r.sh(CAM+0x22)} +24..+2A {[r.sh(CAM+o) for o in (0x24,0x26,0x28,0x2A)]} "
          f"+2C {r.sh(CAM+0x2C)} +4C {r.b(CAM+0x4C)}")
    for i in range(80):
        a = STAGE + i * 0xA4
        if r.b(a) and r.b(a + 1) == 7:
            print("  stage", i, obj(r, a))
    for i in range(48):
        a = MAIN + i * 0x9C
        if r.b(a) and r.b(a + 1) == 6:
            print("  main", i, obj(r, a))


if __name__ == "__main__":
    for s in sys.argv[1:]:
        main(s)
