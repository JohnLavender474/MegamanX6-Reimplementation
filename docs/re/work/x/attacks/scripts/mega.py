from probe import *
M=0x800970A0
for s in STATES:
    r=load(s)
    print(s)
    print(" step", s8(r,M+5), "sub", s8(r,M+6), "anim", u8(r,M+0x17), "var", u8(r,M+2), "form", u8(r,M+0xE7), "+EC", hex(u32(r,M+0xEC)), "+C9", hex(u8(r,M+0xC9)), "game+C9", hex(u8(r,0x800CCF99)))
    print(" 7C..8F", bytes_at(r,M+0x7C,0x14).hex(" "))
    print(" 8E..AF", bytes_at(r,M+0x8E,0x22).hex(" "))
    print(" A8 halfs", [s16(r,M+0xA8+2*i) for i in range(10)])
    print(" x,y", hex(s32(r,M+0x20)), hex(s32(r,M+0x24)), "flip", hex(u8(r,M+0x15)), "+144", u32(r,M+0x144))
