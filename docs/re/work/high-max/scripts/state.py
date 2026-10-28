import sys
sys.path.insert(0, sys.argv[0].rsplit('/',1)[0])
from peek import *
r = load(sys.argv[1] if len(sys.argv)>1 else 'turtloid-8_high-max.state.gz')
G=0x800CCED0
print('stage',u8(r,G+0xC),u8(r,G+0xD),'diff',u8(r,G+0x68),'boss_level',u8(r,G+0x8F),'8D',u8(r,G+0x8D),u8(r,G+0x8E),'hpmax',u8(r,G+0x8C),'boss_choice',u8(r,G+0x43B),'5E',hex(u8(r,G+0x5E)),'5F',hex(u8(r,G+0x5F)),'60',hex(u8(r,G+0x60)),'exit',hex(u8(r,G+0xF)),'26',u8(r,G+0x26),'24',u8(r,G+0x24),'25',u8(r,G+0x25),'2E',u8(r,G+0x2E),'20',hex(u32(r,G+0x20)))
M=0x800970A0
print('mega char',u8(r,M+2),'state',u8(r,M+4),u8(r,M+5),u8(r,M+6),'pos',s16(r,M+0xA),s16(r,M+0xE),'hp',u8(r,M+0x5C))
for i in range(48):
    a=0x8008EF48+i*0x9C
    if u8(r,a)==0: continue
    print(f'main {i} @{a:08x} type {u8(r,a+1)} var {u8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)}/{u8(r,a+7)} pos {s16(r,a+0xA)},{s16(r,a+0xE)}')
    hexd(r,a,0x9C)
for i in range(32):
    a=0x80093C98+i*0xA0
    if u8(r,a)==0: continue
    print(f'vis {i} @{a:08x} type {u8(r,a+1)} var {u8(r,a+2)} st {u8(r,a+4)}/{u8(r,a+5)}/{u8(r,a+6)}/{u8(r,a+7)} pos {s16(r,a+0xA)},{s16(r,a+0xE)} flip {u8(r,a+0x15)} parent {u32(r,a+0x7C):08x}')
    hexd(r,a,0xA0)
