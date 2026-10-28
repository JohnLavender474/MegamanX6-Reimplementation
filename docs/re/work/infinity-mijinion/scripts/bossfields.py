import sys
from probe import load, u8, s8, u16, s16, u32, s32, fx
M0 = 0x8008EF48
G = 0x800CCED0
def mainobj(r, a):
    return (f"  @{a:08x} var {s8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)} act4 {s8(r,a+7)} x,y {s16(r,a+0xA)},{s16(r,a+0xE)} disp {u8(r,a+3)}"
            f" flags {u8(r,a):02x} anim {u8(r,a+0x17)} spr {u8(r,a+0x47)} flip {u8(r,a+0x15)} sx {fx(r,a+0x20)} sy {fx(r,a+0x24)} ax {fx(r,a+0x28)} grav {fx(r,a+0x2C)}"
            f" hp {s8(r,a+0x5C)} cd {u8(r,a+0x60)} if {u8(r,a+0x61)} dmg {u32(r,a+0x58):08x} atk {u32(r,a+0x50):08x} hurt {u32(r,a+0x54):08x} terr {u32(r,a+0x68):08x}\n"
            f"     +7C {s16(r,a+0x7C)} +7E {s16(r,a+0x7E)} +80 {u32(r,a+0x80):08x} +84 {s16(r,a+0x84)} +86 {s8(r,a+0x86)}/{s16(r,a+0x86)} +87 {s8(r,a+0x87)}"
            f" +88 {s8(r,a+0x88)}/{s16(r,a+0x88)} +89 {s8(r,a+0x89)} +8A {s8(r,a+0x8A)} +8B {s8(r,a+0x8B)} +8C {s8(r,a+0x8C)} +8D {s8(r,a+0x8D)} +8E {s8(r,a+0x8E)}"
            f" clut {u16(r,a+0x42):04x} tex {u16(r,a+0x40):04x} prio {u8(r,a+0x16)} coll {u8(r,a+0x70):02x} tbl {u32(r,a+0x30):08x}")
for name in sys.argv[1:]:
    r = load(name)
    print("==", name)
    for i in range(48):
        a = M0 + 0x9C*i
        if u8(r,a) and u8(r,a+1) == 58:
            print(mainobj(r,a))
    print("  da74", hex(u32(r,0x800FDA74)), "da78", [u8(r,0x800FDA78+i) for i in range(3)], "clones da7c", s16(r,0x800FDA7C),
          "copies da7e", s16(r,0x800FDA7E), "da80", s16(r,0x800FDA80), "fight timer da84", u32(r,0x800FDA84))
    print("  game+0x8C", u8(r,G+0x8C), "+0x8D", u8(r,G+0x8D), u8(r,G+0x8E), "lvl", u8(r,G+0x8F), "+0x2E", u8(r,G+0x2E), "+0x24", u8(r,G+0x24),
          "+0x20", hex(u32(r,G+0x20)), "diff", u8(r,G+0x68), "+0x0E", u8(r,G+0x0E), "time", u32(r,G+0x90), "char", u8(r,G+0x38), "effect", u8(r,0x800CD30A))
    print("  cam tgt min x,y", s16(r,0x8009721E), s16(r,0x80097222), "text_open", u8(r,0x8008EAFC), "mega var", u8(r,0x800970A2), "mega step", u8(r,0x800970A5))
