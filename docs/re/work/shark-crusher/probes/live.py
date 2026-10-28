import sys, struct
from pathlib import Path
sys.path.insert(0, str(Path.home()/'IdeaProjects/mmx-demake-tools'))
from mmxdemake.savestate import read_ram
X = Path.home()/'mmx-data/x6'
exe = (X/'extracted/retail/iso/SLUS_013.95').read_bytes()
ram = read_ram(X/'dumps'/sys.argv[1], exe)
def r(a, n): return bytes(ram[a&0x1fffff:(a&0x1fffff)+n])
def h(a): return struct.unpack('<h', r(a,2))[0]
for L in range(3):
    b = 0x800971F8 + L*0x54
    print('layer', L, 'x', h(b+0x0A), 'y', h(b+0x0E), r(b,0x54).hex(' '))
print('game', r(0x800CCED0, 0x30).hex(' '))
m = 0x800970A0
print('mega x,y', h(m+0x0A), h(m+0x0E), 'step', r(m+5,1)[0], 'hp', r(m+0x5C,1).hex(), '+70..', r(m+0x70,4).hex(' '), '+89', r(m+0x89,1).hex(), 'E5 E6', r(m+0xE5,2).hex(' '), 'D5', r(m+0xD5,1).hex(), 'E0', r(m+0xE0,1).hex())
a2 = 0x800CD340
print('actor2', r(a2, 0x9C).hex(' '))
for i in (0, 5, 15, 36, 43):
    a = 0x800C1220 + i*0xA4
    o = r(a, 0xA4)
    print(i, 'var', o[2], 'st', o[4], o[5], o[6], 'disp', o[3], 'x', h(a+0xA), 'y', h(a+0xE), 'y0', h(a+0x86), '+50', o[0x50:0x5C].hex(' '), '+60', o[0x60:0x62].hex(), '+68', o[0x68:0x6C].hex(' '), '+70', o[0x70:0x78].hex(' '), '+7C', o[0x7C:0x88].hex(' '))
for i in range(96):
    a = 0x8009B7B0 + i*0x30
    o = r(a, 0x30)
    if o[0]: print('logic', i, o.hex(' '))
