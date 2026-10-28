import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
X = Path.home()/'mmx-data/x6'
ram = read_ram(X/'dumps'/sys.argv[1], (X/'extracted/retail/iso/SLUS_013.95').read_bytes())
def r(a, n): return bytes(ram[a&0x1fffff:(a&0x1fffff)+n])
def h(a): return struct.unpack('<h', r(a,2))[0]
for name, base, n, size in (('main',0x8008EF48,48,0x9C),('visual',0x80093C98,32,0xA0),('misc',0x800CD410,96,0x60),('misc2',0x800964A0,32,0x60),('effect',0x80091298,96,0x70),('weapon',0x800950A0,32,0xA0)):
    for i in range(n):
        a = base+i*size; o = r(a,size)
        if o[0]: print(name, i, hex(a), 'type', o[1], 'var', o[2], 'st', o[4:7].hex(' '), 'x', h(a+0xA), 'y', h(a+0xE), o[:0x20].hex(' '))
