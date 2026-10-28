import sys; sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
r = load(sys.argv[1] if len(sys.argv)>1 else STATE)
dump = '-d' in sys.argv
G=0x800CCED0
print('stage',u8(r,G+0xC),u8(r,G+0xD),'diff',u8(r,G+0x68),'boss_level',u8(r,G+0x8F),'hpmax',u8(r,G+0x8C),'boss_choice',u8(r,G+0x43B),'5E',hex(u8(r,G+0x5E)),'5F',hex(u8(r,G+0x5F)),'60',hex(u8(r,G+0x60)),'exit',hex(u8(r,G+0xF)),'boss',hex(u32(r,G+0x20)),'24',u8(r,G+0x24),'25',u8(r,G+0x25),'armors',hex(u8(r,G+0x5F)))
M=0x800970A0
print('mega char',u8(r,M+2),'state',u8(r,M+4),u8(r,M+5),u8(r,M+6),u8(r,M+7),'pos',s16(r,M+0xA),s16(r,M+0xE),'hp',hex(u8(r,M+0x5C)),'iframes',u8(r,M+0x61),'hitlock',u8(r,M+0xA4),'A5',hex(u8(r,M+0xA5)),'nohit',u8(r,M+0x7A),'parts',hex(u32(r,M+0xEC)))
pools=[('main',0x8008EF48,48,0x9C),('stage',0x800C1220,80,0xA4),('vis',0x80093C98,32,0xA0),('misc',0x800CD410,96,0x60),('misc2',0x800964A0,32,0x60),('eff',0x80091298,96,0x70),('weap',0x800950A0,32,0xA0),('logic',0x8009B7B0,96,0x30),('ray',0x8009C9B0,64,0x60),('k7',0x800C4778,4,0x30),('actor2',0x800CD340,1,0xB0)]
for name,base,n,sz in pools:
    for i in range(n):
        a=base+i*sz
        if u8(r,a)==0: continue
        extra=''
        if sz>=0x60 and name not in ('logic',):
            extra=f' hp {s8(r,a+0x5C)} cd {s8(r,a+0x60)} dt {u32(r,a+0x58):08x} atk {u32(r,a+0x50):08x} hurt {u32(r,a+0x54):08x} anim {u32(r,a+0x30):08x} spr {u8(r,a+0x47)} tex {u16(r,a+0x40):04x} pal {u16(r,a+0x42):04x}'
        print(f'{name} {i} @{a:08x} f {u8(r,a):02x} type {u8(r,a+1)} var {u8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)}/{u8(r,a+7)} pos {s16(r,a+0xA)},{s16(r,a+0xE)}'+extra)
        if dump and name in sys.argv: hexd(r,a,sz)
