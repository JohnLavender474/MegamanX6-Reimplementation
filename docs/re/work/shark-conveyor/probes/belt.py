from common import *
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
name = sys.argv[1]
st = read_state(X/'dumps'/name); sp = scratchpad(st)
s = R(name)
spw = lambda o: struct.unpack_from('<I', sp, o)[0]
lay, scr, tdef = spw(4), spw(8), spw(0xC)
W, Hh = s.b(0x800CD338), s.b(0x800CD339)
stride = s.H(0x8008EC10)
print('map', W, 'x', Hh, 'screens; stride', stride, 'layout', hex(lay), 'screens', hex(scr), 'tiledefs', hex(tdef))
def screen(layer, sx, sy):
    if sx >= W or sy >= Hh: return 0
    return s.b(lay + layer*stride + W*sy + sx)
def tile(layer, x, y):
    sc = screen(layer, x>>8, y>>8)
    if sc == 0: return None, None
    t = s.H(scr + sc*512 + ((y>>4)&15)*32 + ((x>>4)&15)*2) & 0x3fff
    return t, s.b(tdef + 4*t)
cells = {}
for ty in range(Hh*16):
    for tx in range(W*16):
        t, c = tile(0, tx*16, ty*16)
        if c == 58: cells[(tx, ty)] = t
rows = {}
for (tx, ty), t in sorted(cells.items(), key=lambda k: (k[0][1], k[0][0])):
    rows.setdefault(ty, []).append(tx)
for ty, xs in rows.items():
    runs = []; a = b = xs[0]
    for x in xs[1:]:
        if x == b+1: b = x
        else: runs.append((a, b)); a = b = x
    runs.append((a, b))
    print('value 58 row y', ty*16, '-', ty*16+15, ':', ', '.join(f'x {a*16}-{b*16+15}' for a, b in runs))
print('tiles with value 58:', sorted(set(cells.values())))
print('value counts near belt rows:')
print('saved 2x2 blocks 0x800F8C08:')
for i in range(8):
    x, y = s.H(0x800F8C08+4*i), s.H(0x800F8C0C-4+4*i+2) if False else s.H(0x800F8C08+4*i+2)
    print(' ', i, (x, y), 'tiles', [s.H(0x800F981C+8*i+2*k) for k in range(4)], 'now', [tile(0, x+dx, y+dy) for dy in (0,16) for dx in (0,16)])
print('frame 0x800F985C', s.H(0x800F985C), ' 0x800E3142', s.H(0x800E3142), ' frame count 0x80097420', s.w(0x80097420))
print('copy regions 0x800F88E8 (dx, dy, w, h) and source by frame (0x800F8988 + frame*4 + i*32):')
for i in range(20):
    a = 0x800F88E8 + 8*i
    dx, dy, w, h = s.h(a), s.h(a+2), s.H(a+4), s.H(a+6)
    srcs = [(s.H(0x800F8988+f*4+i*32), s.H(0x800F8988+f*4+i*32+2)) for f in range(8)]
    print(f'  {i:2} dst ({dx},{dy}) {w}x{h} src {srcs}  dst value {tile(0, dx, dy)}')
