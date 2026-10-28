import sys, struct, os
from mmxdemake.savestate import read_ram
EXE = open(os.path.expanduser('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95'), 'rb').read()
CLASSES = [('main', 0x8008EF48, 48, 0x9C), ('stage', 0x800C1220, 80, 0xA4), ('visual', 0x80093C98, 32, 0xA0),
           ('misc', 0x800CD410, 96, 0x60), ('misc2', 0x800964A0, 32, 0x60), ('weapon', 0x800950A0, 32, 0xA0),
           ('logic', 0x8009B7B0, 96, 0x30)]
def u8(r, a): return r[a & 0x1fffff]
def s16(r, a): return struct.unpack_from('<h', r, a & 0x1fffff)[0]
def u16(r, a): return struct.unpack_from('<H', r, a & 0x1fffff)[0]
def u32(r, a): return struct.unpack_from('<I', r, a & 0x1fffff)[0]
G = 0x800CCED0
C = 0x800971F8
for st in sys.argv[1:]:
    r = read_ram(os.path.expanduser('~/mmx-data/x6/dumps/' + st), EXE)
    print('==', st, 'stage', u8(r, G + 0xC), 'area', u8(r, G + 0xD), 'cp', u8(r, G + 0x1D), 'form', u8(r, G + 0x5E),
          'diff', u8(r, G + 0x68), 'ne', u8(r, 0x800CD30A), 'boss_choice', u8(r, 0x800CD30B),
          'parts', hex(u8(r, G + 0x69)), 'flags26-28', u8(r, G + 0x26), u8(r, G + 0x27), u8(r, G + 0x28),
          'exit', hex(u8(r, G + 0xF)), 'size', u8(r, 0x800CD338), u8(r, 0x800CD339), 'eab0', u8(r, 0x8008EAB0))
    print('   mega x,y', s16(r, 0x800970A0 + 0xA), s16(r, 0x800970A0 + 0xE), 'step', u8(r, 0x800970A0 + 5))
    for l in range(3):
        c = C + l * 0x54
        print(f'   cam{l} x,y {s16(r, c + 0xA)},{s16(r, c + 0xE)} cur maxx {u16(r, c + 0x1C)} minx {u16(r, c + 0x1E)} '
              f'maxy {u16(r, c + 0x20)} miny {u16(r, c + 0x22)}; tgt maxx {u16(r, c + 0x24)} minx {u16(r, c + 0x26)} '
              f'maxy {u16(r, c + 0x28)} miny {u16(r, c + 0x2A)} ease {u8(r, c + 0x47)}')
    for name, base, n, sz in CLASSES:
        for i in range(n):
            a = base + i * sz
            if u8(r, a) == 0:
                continue
            b = r[a & 0x1fffff:(a & 0x1fffff) + sz]
            rec = u32(r, a + 0x10) if name in ('main', 'stage', 'logic') else 0
            print(f'  {name}[{i}] {b[0]:02x} type{b[1]} var{b[2]} disp{b[3]} st{b[4]}/{b[5]}/{b[6]} '
                  f'x{s16(r, a + 10)} y{s16(r, a + 14)} rec{rec:#x}')
