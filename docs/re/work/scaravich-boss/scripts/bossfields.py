import sys
from probe import load, u8, s8, u16, s16, u32, s32
B = 0x8008EF48
V = 0x80093C98
for name in sys.argv[1:]:
    r = load(name)
    print("==", name)
    print("  boss +0x82 hp_prev", s16(r, B+0x82), "+0x84", s8(r, B+0x84), "+0x85", u8(r, B+0x85), "+0x86", u8(r, B+0x86),
          "+0x87 cycles", s8(r, B+0x87), "+0x88 ball_level", s8(r, B+0x88), "+0x89", s8(r, B+0x89), "+0x8A", s8(r, B+0x8A),
          "+0x8B", s8(r, B+0x8B), "+0x9A ball_var", s8(r, B+0x9A), "+0x7C", s16(r, B+0x7C), "+0x7E", s16(r, B+0x7E), "+0x80", s16(r, B+0x80))
    print("  globals 6af0", s16(r, 0x800F6AF0), "6af2", s16(r, 0x800F6AF2), "6af4 fight_time", u32(r, 0x800F6AF4), "ofs", s16(r, 0x800F6AF8), s16(r, 0x800F6AFA))
    for i in range(32):
        a = V + 0xA0 * i
        if u8(r, a) and u8(r, a+1) == 14:
            print(f"  visual14 slot {i} var {s8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)} act4 {s8(r,a+7)} x {s16(r,a+0xA)} y {s16(r,a+0xE)} anim {u8(r,a+0x17)} hp {s8(r,a+0x5C)} cd {u8(r,a+0x60)}"
                  f" +0x90 {s16(r,a+0x90)} +0x92 floor_y {s16(r,a+0x92)} +0x94 {u8(r,a+0x94)} +0x95 {u8(r,a+0x95)} +0x96 {u8(r,a+0x96)} dmg {u32(r,a+0x58):08x} prio {u8(r,a+0x16)}")
