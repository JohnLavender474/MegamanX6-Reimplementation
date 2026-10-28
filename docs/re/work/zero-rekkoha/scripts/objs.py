from zprobe import *
r=load("heatnix-3_rekkoha")
for name,base,n,sz in [("main",0x8008EF48,48,0x9C),("stage",0x800C1220,80,0xA4),("visual",0x80093C98,32,0xA0)]:
    for i in range(n):
        a=base+sz*i
        if u8(r,a):
            print(name,i,"flag",hex(u8(r,a)),"type",u8(r,a+1),"var",s8(r,a+2),"st",u8(r,a+4),u8(r,a+5),"x",u16(r,a+0xA),"y",u16(r,a+0xE),"hp",s8(r,a+0x5C) if sz>0x5C else None,"tbl58",hex(u32(r,a+0x58)),"65",u8(r,a+0x65),"63",u8(r,a+0x63))
