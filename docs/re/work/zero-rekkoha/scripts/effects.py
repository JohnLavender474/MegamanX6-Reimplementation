import sys
from zprobe import *
r=load(sys.argv[1] if len(sys.argv)>1 else "heatnix-3_rekkoha")
for i in range(96):
    a=0x80091298+0x70*i
    if u8(r,a):
        print("eff",i,"flag",hex(u8(r,a)),"type",u8(r,a+1),"var",s8(r,a+2),"st",u8(r,a+4),u8(r,a+5),u8(r,a+6),"x",fx(r,a+8),"y",fx(r,a+12),"anim",u8(r,a+0x17),"tbl",hex(u32(r,a+0x30)),"38",hex(u32(r,a+0x38)),"3C",hex(u32(r,a+0x3C)),"40",hex(u16(r,a+0x40)),"42",hex(u16(r,a+0x42)),"spr",u8(r,a+0x47))
        print("   ",bytes_at(r,a,0x70).hex(" "))
