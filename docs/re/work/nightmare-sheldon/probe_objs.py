import sys, struct, os
sys.path.insert(0, os.path.expanduser('~/IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'),'rb').read()
CLASSES = [('main',0x8008EF48,48,0x9C),('stage',0x800C1220,80,0xA4),('visual',0x80093C98,32,0xA0),
           ('misc',0x800CD410,96,0x60),('misc2',0x800964A0,32,0x60),('logic',0x8009B7B0,96,0x30),('ray',0x8009C9B0,64,0x60)]
def u8(r,a): return r[a&0x1fffff]
def s16(r,a): return struct.unpack_from('<h',r,a&0x1fffff)[0]
def u32(r,a): return struct.unpack_from('<I',r,a&0x1fffff)[0]
for st in sys.argv[1:]:
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/'+st), EXE)
    print('==', st, 'stage', u8(r,0x800CCEDC), 'area', u8(r,0x800CCEDD), 'nightmare_effect', u8(r,0x800CD30A),
          'diff', u8(r,0x800CCF38), 'mega x,y', s16(r,0x800970A0+0xA), s16(r,0x800970A0+0xE), 'hp', u8(r,0x800970A0+0x5C))
    for name, base, n, sz in CLASSES:
        for i in range(n):
            a = base + i*sz
            if u8(r,a) == 0: continue
            b = r[a&0x1fffff:(a&0x1fffff)+sz]
            extra=''
            if name in ('main','stage','visual'):
                extra = f"st{b[4]} step{b[5]} s2{b[6]} b7={b[7]} x{s16(r,a+10)} y{s16(r,a+14)} disp{b[3]} hp{b[0x5c]} cd{b[0x60]} flip{b[0x15]:#x} rec{u32(r,a+0x10):#x}"
            elif name=='logic':
                extra = f"st{b[4]} step{b[5]} x{s16(r,a+10)} y{s16(r,a+14)}"
            else:
                extra = f"st{b[4]} x{s16(r,a+10)} y{s16(r,a+14)}"
            print(f"  {name}[{i}] @{a:#x}: {b[0]:02x} type{b[1]} var{b[2]} sub{b[3]}  {extra}")
