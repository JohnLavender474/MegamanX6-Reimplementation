import sys
from probe import load, u8, s8, u16, s16, u32, s32, fx
B = 0x8008EF48
V = 0x80093C98
E = 0x80091298
G = 0x800CCED0
for name in sys.argv[1:]:
    r = load(name)
    print("==", name)
    print("  boss st", u8(r,B+4), u8(r,B+5), u8(r,B+6), "act4", s8(r,B+7), "x,y", s16(r,B+0xA), s16(r,B+0xE), "disp", u8(r,B+3),
          "anim", u8(r,B+0x17), "spr", u8(r,B+0x47), "flip", u8(r,B+0x15), "sx", fx(r,B+0x20), "sy", fx(r,B+0x24), "grav", fx(r,B+0x2C),
          "hp", s8(r,B+0x5C), "cd", u8(r,B+0x60), "dmg", hex(u32(r,B+0x58)), "atk", hex(u32(r,B+0x50)), "hurt", hex(u32(r,B+0x54)), "terr", hex(u32(r,B+0x68)))
    print("  +7C", s16(r,B+0x7C), "+7E", s16(r,B+0x7E), "+80", hex(u32(r,B+0x80)), "+84", s16(r,B+0x84), "+86", s16(r,B+0x86),
          "+88", s16(r,B+0x88), "+8A", s8(r,B+0x8A), "+8B", s8(r,B+0x8B), "+8C", s8(r,B+0x8C), "+8D", s8(r,B+0x8D), "clut", hex(u16(r,B+0x42)), "tex", hex(u16(r,B+0x40)), "prio", u8(r,B+0x16), "coll", hex(u8(r,B+0x70)))
    print("  fight timer 0x800f7840", u32(r,0x800F7840), " game+0x8C", u8(r,G+0x8C), "+0x8D", u8(r,G+0x8D), "+0x8F lvl", u8(r,G+0x8F), "+0x2E", u8(r,G+0x2E), "+0x24", u8(r,G+0x24), "+0x25", u8(r,G+0x25), "diff", u8(r,G+0x68), "+0x0E", u8(r,G+0x0E))
    print("  fade_r", u16(r,0x800CF854), "fade_g", u16(r,0x80097430), "fade_b", u16(r,0x80090D5C), "fade_mode", u8(r,0x800CF850), "fade_mask", [hex(u32(r,0x80090C90+4*i)) for i in range(16)])
    print("  text_open", u8(r,0x8008EAFC), "mega+0x02", u8(r,0x800970A2), "mega step", u8(r,0x800970A5))
    for i in range(32):
        a = V + 0xA0*i
        if u8(r,a):
            print(f"  visual{i} type {u8(r,a+1)} var {s8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)} x {s16(r,a+0xA)} y {s16(r,a+0xE)} anim {u8(r,a+0x17)} spr {u8(r,a+0x47)} sx {fx(r,a+0x20)} sy {fx(r,a+0x24)} grav {fx(r,a+0x2C)} hp {s8(r,a+0x5C)} cd {u8(r,a+0x60)} dmg {u32(r,a+0x58):08x} parent {u32(r,a+0x7C):08x}")
    for i in range(96):
        a = E + 0x70*i
        if u8(r,a):
            print(f"  effect{i} type {u8(r,a+1)} var {s8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)} x {s16(r,a+0xA)} y {s16(r,a+0xE)}")
