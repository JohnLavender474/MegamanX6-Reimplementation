import os
import struct
import sys

sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram

EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
MAIN = (0x8008EF48, 48, 0x9C)
VISUAL = (0x80093C98, 32, 0xA0)
MEGA = 0x800970A0


def off(a):
    return a & 0x1FFFFF


def s16(r, a):
    return struct.unpack_from('<h', r, off(a))[0]


def s32(r, a):
    return struct.unpack_from('<i', r, off(a))[0]


def u32(r, a):
    return struct.unpack_from('<I', r, off(a))[0]


def show(r, a, sz):
    b = r[off(a):off(a) + sz]
    print(f"    flags {b[0]:02x} type {b[1]} var {b[2]} disp {b[3]} state {b[4]} step {b[5]} step2 {b[6]} b7 {b[7]}")
    print(f"    x {s32(r, a + 8) / 65536:.4f} y {s32(r, a + 12) / 65536:.4f} layer {b[0x14]} flip {b[0x15]:#x} prio {b[0x16]}")
    print(f"    speed_x {s32(r, a + 0x20) / 65536:.5f} speed_y {s32(r, a + 0x24) / 65536:.5f} "
          f"accel_x {s32(r, a + 0x28) / 65536:.5f} gravity {s32(r, a + 0x2C) / 65536:.5f}")
    print(f"    anim_tbl {u32(r, a + 0x30):#x} anime +44.. {b[0x44]:02x} {b[0x45]:02x} {b[0x46]:02x} {b[0x47]:02x}")
    print(f"    boxes atk {u32(r, a + 0x50):#x} hurt {u32(r, a + 0x54):#x} dmg {u32(r, a + 0x58):#x} terr {u32(r, a + 0x68):#x}")
    print(f"    hp {b[0x5C]} contact {b[0x60]} iframes {b[0x61]} latch {b[0x67]} collide {b[0x70]:#x} rec {u32(r, a + 0x10):#x}")
    print("    +0x78..: " + ' '.join(f"{x:02x}" for x in b[0x78:sz]))


def main(states, types, vtypes):
    for st in states:
        r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
        print(f"== {st}: stage {r[off(0x800CCEDC)]} area {r[off(0x800CCEDD)]} diff {r[off(0x800CCF38)]} "
              f"frame? mega x {s16(r, MEGA + 10)} y {s16(r, MEGA + 14)} flip {r[off(MEGA + 0x15)]:#x} "
              f"cam {s16(r, 0x80097202)},{s16(r, 0x80097206)}")
        base, n, sz = MAIN
        for i in range(n):
            a = base + i * sz
            if r[off(a)] and r[off(a) + 1] in types:
                print(f"  main[{i}] @{a:#x}")
                show(r, a, sz)
                rec = u32(r, a + 0x10)
                if rec:
                    print("    record: " + ' '.join(f"{x:02x}" for x in r[off(rec):off(rec) + 8]))
        base, n, sz = VISUAL
        for i in range(n):
            a = base + i * sz
            if r[off(a)] and r[off(a) + 1] in vtypes:
                print(f"  visual[{i}] @{a:#x} parent {u32(r, a + 0x7C):#x}")
                show(r, a, sz)


if __name__ == '__main__':
    t = [int(x) for x in sys.argv[1].split(',')]
    v = [int(x) for x in sys.argv[2].split(',')]
    main(sys.argv[3:], t, v)
