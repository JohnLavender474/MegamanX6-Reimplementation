from zprobe import *
M=0x800970A0; G=0x800CCED0
for s in STATES:
    r=load(s)
    print("==",s, "stage",u16(r,G+0x0C), "char",u8(r,G+0x38),"form5E",u8(r,G+0x5E),"armors5F",hex(u8(r,G+0x5F)),"g60",hex(u8(r,G+0x60)),
          "var",s8(r,M+2),"state",u8(r,M+4),"step",u8(r,M+5),u8(r,M+6),"act4",s8(r,M+7),"anim",u8(r,M+0x17),"flag",hex(u8(r,M+0x45)),"dur",s8(r,M+0x44),"next",s8(r,M+0x46))
    print("  C9",hex(u8(r,M+0xC9)),"CB",s8(r,M+0xCB),"8E",s8(r,M+0x8E),"86",s8(r,M+0x86),"93",s8(r,M+0x93),"A6",s8(r,M+0xA6),"E7",u8(r,M+0xE7),"parts",hex(u32(r,M+0xEC)),"hp",s8(r,M+0x5C),
          "energy",[s16(r,M+0xA8+2*i) for i in range(10)],"max14A",s16(r,M+0x14A),"144",u32(r,M+0x144),"x",fx(r,M+8),"y",fx(r,M+12),"sx",fx(r,M+0x20),"sy",fx(r,M+0x24),"flip",hex(u8(r,M+0x15)),"7A",u8(r,M+0x7A),"D3",s8(r,M+0xD3),"key",hex(u16(r,M+0x7C)),hex(u16(r,M+0x80)))
    print("  phys", [fx(r,M+o) for o in range(0xF0,0x138,4)])
    for i in range(32):
        a=0x800950A0+0xA0*i
        if u8(r,a):
            print("  wpn",i,"flag",hex(u8(r,a)),"type",u8(r,a+1),"var",s8(r,a+2),"st",u8(r,a+4),u8(r,a+5),"x",fx(r,a+8),"y",fx(r,a+12),"anim",u8(r,a+0x17),"aflag",u8(r,a+0x45),"tag",u8(r,a+0x64),"+84",u32(r,a+0x84),"box",hex(u32(r,a+0x50)),"9C",u8(r,a+0x9C),"8C",u32(r,a+0x8C),"90",u32(r,a+0x90))
