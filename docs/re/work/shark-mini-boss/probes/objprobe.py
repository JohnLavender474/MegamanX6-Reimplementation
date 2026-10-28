import sys, struct, gzip
sys.path.insert(0, __file__.rsplit("/",1)[0])
from pathlib import Path
D = Path.home()/'mmx-data/x6/dumps'
EXT = Path.home()/'mmx-data/x6/extracted/retail'
def ram(name):
    d = gzip.decompress((D/name).read_bytes())
    return d[0x9025:0x9025+0x200000]
def at(r, a, n): o = a & 0x1fffff; return r[o:o+n]
def u8(r,a): return at(r,a,1)[0]
def s16(r,a): return struct.unpack('<h',at(r,a,2))[0]
def u16(r,a): return struct.unpack('<H',at(r,a,2))[0]
def u32(r,a): return struct.unpack('<I',at(r,a,4))[0]
def s32(r,a): return struct.unpack('<i',at(r,a,4))[0]
def dump(r, base, size, i):
    a = base + i*size
    return at(r, a, size)
def hexrow(b): return ' '.join(f'{x:02x}' for x in b)
if __name__ == '__main__':
    name = sys.argv[1]
    r = ram(name)
    ov = (EXT/'rock_bin/07.bin').read_bytes()
    m = at(r, 0x800E9860, len(ov))
    diff = sum(1 for a,b in zip(ov,m) if a!=b)
    print(f'overlay 07 at 0x800E9860: {len(ov)} bytes, {diff} differ, end {0x800E9860+len(ov):#x}')
    g = 0x800CCED0
    print('stage/area', u8(r,g+0xC), u8(r,g+0xD), 'checkpoint', u8(r,g+0x1D), 'exit', hex(u8(r,g+0xF)),
          'diff', u8(r,g+0x68), 'form', u8(r,g+0x5E), 'bosslvl', u8(r,g+0x8F), 'cnt8D', u8(r,g+0x8D), u8(r,g+0x8E),
          'hpmax', u8(r,g+0x8C), 'g24', u8(r,g+0x24), 'g20', hex(u32(r,g+0x20)), 'g26..28', u8(r,g+0x26),u8(r,g+0x27),u8(r,g+0x28),
          'stop', hexrow(at(r,g+0x10,11)), 'lock', u8(r,g+0x1C))
    mg = 0x800970A0
    print('mega x,y', s16(r,mg+0xA), s16(r,mg+0xE), 'state/step', u8(r,mg+4), u8(r,mg+5), u8(r,mg+6), 'hp', u8(r,mg+0x5C), 'D0..D2', hexrow(at(r,mg+0xD0,3)), '7A', u8(r,mg+0x7A))
    c = 0x800971F8
    print('cam x,y', s16(r,c+0xA), s16(r,c+0xE), 'lim', [s16(r,c+0x1C+2*k) for k in range(4)], 'tgt', [s16(r,c+0x24+2*k) for k in range(4)])
    for cls, base, size, n in (('main',0x8008EF48,0x9C,48),('stage',0x800C1220,0xA4,80),('visual',0x80093C98,0xA0,32),('sys',0x8009B7B0,0x30,96),('effect',0x80091298,0x70,96)):
        for i in range(n):
            b = dump(r, base, size, i)
            if b[0] == 0: continue
            if cls in ('main','stage','visual'):
                print(f'{cls}[{i}] @{base+i*size:#x} fl {b[0]:02x} t {b[1]} var {b[2]} disp {b[3]} st {b[4]}/{b[5]}/{b[6]}/{b[7]} x {struct.unpack_from("<h",b,0xA)[0]} y {struct.unpack_from("<h",b,0xE)[0]} hp {b[0x5C]} cd {b[0x60]} if {b[0x61]} tbl {struct.unpack_from("<I",b,0x58)[0]:#x} ab {hexrow(b[0x50:0x54])}/{struct.unpack_from("<I",b,0x50)[0]:#x} hb {struct.unpack_from("<I",b,0x54)[0]:#x} tb {struct.unpack_from("<I",b,0x68)[0]:#x} pri {b[0x16]} flip {b[0x15]:02x} an {hexrow(b[0x44:0x48])}')
                print('   7c..:', hexrow(b[0x7C:size]))
            else:
                print(f'{cls}[{i}] @{base+i*size:#x}', hexrow(b[:0x30]))
