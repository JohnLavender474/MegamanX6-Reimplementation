from common import *
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
name = sys.argv[1]
st = read_state(X/'dumps'/name); sp = scratchpad(st); s = R(name)
spw = lambda o: struct.unpack_from('<I', sp, o)[0]
lay, scr, tdef = spw(4), spw(8), spw(0xC)
W, Hh = s.b(0x800CD338), s.b(0x800CD339); stride = s.H(0x8008EC10)
def col(x, y):
    if x < 0 or y < 0 or x >= W*256 or y >= Hh*256: return 0
    sc = s.b(lay + W*(y>>8) + (x>>8))
    if sc == 0: return 0
    t = s.H(scr + sc*512 + ((y>>4)&15)*32 + ((x>>4)&15)*2) & 0x3fff
    v = s.b(tdef + 4*t); return 56 if v == 59 else v
prof = []
for x in range(0, 4480, 16):
    f = next((y for y in range(240, 480, 16) if col(x, y) >= 56), None)
    prof.append((x, f, col(x, f) if f is not None else None))
runs = []
for x, f, v in prof:
    if runs and runs[-1][2] == f and runs[-1][3] == v: runs[-1][1] = x+15
    else: runs.append([x, x+15, f, v])
print('floor tops under the press (x 0-4479, first cell value >= 56 from y 240):')
for a, b, f, v in runs: print(f'  x {a}-{b}: floor y {f} (value {v})')
ceil = sorted({y for x in range(0, 4480, 16) for y in range(0, 320, 16) if col(x, y) == 57})
print('rows with value 57 above y 320:', ceil)
print('type 9 bodies (state offset - var =', ')')
for i in range(80):
    a = 0x800C1220 + i*0xA4; o = s.r(a, 0xA4)
    if o[0] and o[1] == 9:
        b = s.r(s.w(a+0x68), 4); ox_, oy_, w, h = struct.unpack('bbBB', b)
        y0 = s.h(a+0x86); x = s.h(a+0xA)
        fl = sorted({f for xx, f, v in prof if x+ox_-w <= xx+8 < x+ox_+w})
        print(f'  slot {i} var {o[2]} x {x+ox_-w}-{x+ox_+w-1} y0 {y0} box {(ox_,oy_,w,h)} bottom top-pos {y0+oy_+h} bottom-pos {y0+80+oy_+h} floors under {fl}')
