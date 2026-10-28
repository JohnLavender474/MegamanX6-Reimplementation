import sys, struct, zlib
sys.path.insert(0, '/home/johnlavender/IdeaProjects/mmx-demake-tools')
from mmxdemake.savestate import read_state, ram_from
from pathlib import Path
EXE = Path('~/mmx-data/x6/extracted/retail/iso/SLUS_013.95').expanduser().read_bytes()

def png(path, w, h, rgb):
    raw = b''.join(b'\0' + rgb[y*w*3:(y+1)*w*3] for y in range(h))
    def chunk(t, d):
        c = struct.pack('>I', len(d)) + t + d
        return c + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    Path(path).write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))

def main(p, out=None):
    st = read_state(p); ram = ram_from(st, EXE, p)
    r = lambda a, n: ram[(a & 0x1FFFFF):(a & 0x1FFFFF) + n]
    u8 = lambda a: r(a, 1)[0]
    s16 = lambda a: struct.unpack('<h', r(a, 2))[0]
    u32 = lambda a: struct.unpack('<I', r(a, 4))[0]
    if out:
        pre = st[0x25:0x25 + 128*96*3]
        rgb = bytes(b for i in range(0, len(pre), 3) for b in (pre[i+2], pre[i+1], pre[i]))
        png(out, 128, 96, rgb)
    g = 0x800CCED0
    print('stage', u8(g+0x0C) if False else '', 'game bytes 0..0x30:', r(g, 0x30).hex())
    for L, base in enumerate((0x800971F8, 0x8009724C, 0x800972A0)):
        print(f'cam{L} mode {u8(base+4)} pos ({s16(base+0x0A)},{s16(base+0x0E)}) lim maxx {s16(base+0x1C)} minx {s16(base+0x1E)} maxy {s16(base+0x20)} miny {s16(base+0x22)} off ({s16(base+0x40)},{s16(base+0x42)})')
    m = 0x800970A0
    print('mega x,y', s16(m+0x0A), s16(m+0x0E), 'step', r(m+4, 4).hex(), 'hp', u8(m+0x4E) if False else '')
    def dump(name, base, n, size, xo=0x0A, yo=0x0E):
        for i in range(n):
            a = base + i*size
            if u8(a) == 0: continue
            o = r(a, size)
            print(f'{name}[{i:2}] {a:08x} f={o[0]:02x} t={o[1]:3} var={o[2]:3} st={o[4]}.{o[5]}.{o[6]}.{o[7]} pos=({s16(a+xo)},{s16(a+yo)}) | {o[:0x20].hex()} | {o[0x20:0x60].hex()}')
    dump('main', 0x8008EF48, 48, 0x9C)
    dump('stage', 0x800C1220, 80, 0xA4)
    dump('vis', 0x80093C98, 32, 0xA0)
    dump('eff', 0x80091298, 96, 0x70)
    dump('misc', 0x800CD410, 96, 0x60)
    dump('misc2', 0x800964A0, 32, 0x60)
    dump('logic', 0x8009B7B0, 96, 0x30)
    dump('ray', 0x8009C9B0, 64, 0x60)
    dump('k7', 0x800C4778, 4, 0x30)
    dump('actor2', 0x800CD340, 1, 0xB0)
if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
