import sys
from zprobe import *
M=0x800970A0; G=0x800CCED0
s=sys.argv[1] if len(sys.argv)>1 else "heatnix-3_rekkoha"
r=load(s)
print("stage",u16(r,G+0x0C),"area",u8(r,G+0x0E) if True else 0,"char",u8(r,G+0x38),"g5F",hex(u8(r,G+0x5F)),"g60",hex(u8(r,G+0x60)))
print("mega: var",s8(r,M+2),"state",u8(r,M+4),"step",u8(r,M+5),u8(r,M+6),u8(r,M+7),"anim",u8(r,M+0x17),"frame",u8(r,M+0x16) ,"flag",hex(u8(r,M+0x45)),"dur",s8(r,M+0x44))
print("  C9",hex(u8(r,M+0xC9)),"CB",s8(r,M+0xCB),"8E",s8(r,M+0x8E),"93",s8(r,M+0x93),"E4",u8(r,M+0xE4),"hp",s8(r,M+0x5C),"energy",[s16(r,M+0xA8+2*i) for i in range(10)],"x",fx(r,M+8),"y",fx(r,M+12),"flip",hex(u8(r,M+0x15)))
print("  raw 0x80..0xA0", bytes_at(r,M+0x80,0x20).hex(" "))
print("  raw 0xC0..0x100", bytes_at(r,M+0xC0,0x40).hex(" "))
for i in range(32):
    a=0x800950A0+0xA0*i
    if u8(r,a):
        print("wpn",i,"flag",hex(u8(r,a)),"type",u8(r,a+1),"var",s8(r,a+2),"st",u8(r,a+4),u8(r,a+5),u8(r,a+6),u8(r,a+7),"x",fx(r,a+8),"y",fx(r,a+12),"anim",u8(r,a+0x17),"tbl",hex(u32(r,a+0x30)),"38",hex(u32(r,a+0x38)),"3C",hex(u32(r,a+0x3C)),"40",hex(u16(r,a+0x40)),"42",hex(u16(r,a+0x42)),"sprite",u8(r,a+0x47),"aflag",u8(r,a+0x45),"tag",u8(r,a+0x64))
        print("   ", bytes_at(r,a,0xA0).hex(" "))
