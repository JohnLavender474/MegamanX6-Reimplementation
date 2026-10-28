from common import *
s = R(sys.argv[1]); m = 0x800970A0
mx, my = s.h(m+0xA), s.h(m+0xE); bx = s.w(m+0x68)
box = s.r(bx, 4); oy = struct.unpack('b', box[1:2])[0]
print('X', mx, my, 'box', list(struct.unpack('bbBB', box)), 'top', my+oy-box[3], 'bottom', my+oy+box[3])
k = 0x800C4778+0x30
print('press var', s.b(k+2), 'state', s.b(k+4), 'sub', s.b(k+5), 'timer', s.H(k+0x14), 'count', s.H(k+0x16), 'speed', s.i(k+0x18)/65536, 'offset', s.i(k+0x1C)/65536, 'saved', s.i(k+0x20)/65536)
l0, l1 = s.h(0x800971F8+0xE), s.h(0x8009725A)
print('layer0 y', l0, 'layer1 y', l1, 'layer1 old y', s.h(0x80097266), 'kill line (RAM now)', l0-l1+240, 'kill line = offset - var + 240 =', s.i(k+0x1C)//65536 - s.b(k+2) + 240)
for i in range(80):
    a = 0x800C1220 + i*0xA4; o = s.r(a, 0xA4)
    if o[0] and o[1] == 9 and abs(s.h(a+0xA)-mx) < 300:
        b = s.r(s.w(a+0x68), 4) if s.w(a+0x68) else b'\0'*4
        ox_, oy_, w, h = struct.unpack('bbBB', b)
        y = s.h(a+0xE)
        print(' type 9', i, 'var', o[2], 'x', s.h(a+0xA), 'y', y, 'y0', s.h(a+0x86), 'box', (ox_, oy_, w, h), 'bottom', y+oy_+h, 'x range', s.h(a+0xA)+ox_-w, s.h(a+0xA)+ox_+w, '+72', o[0x72], '+76', o[0x76])
