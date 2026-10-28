from common import *
for st in sys.argv[1:]:
    s = R(st)
    print('==', st)
    print('game', s.r(0x800CCED0, 0x30).hex(' '))
    m = 0x800970A0
    print('mega x,y', s.h(m+0xA), s.h(m+0xE), 'step', s.r(m+4,4).hex(' '), 'hp', s.b(m+0x5C), '+68', s.r(m+0x68,4).hex(' '), '+70', s.r(m+0x70,4).hex(' '), 'sx', s.i(m+0x50)/65536, s.i(m+0x54)/65536, '+89', s.b(m+0x89), 'E5 E6', s.b(m+0xE5), s.b(m+0xE6), '+138', s.r(m+0x138,4).hex(' '))
    for L in range(3):
        bb = 0x800971F8 + L*0x54
        print(' layer', L, 'x', s.h(bb+0xA), 'y', s.h(bb+0xE), 'mode', s.b(bb+4))
    for k in range(4):
        a = 0x800C4778 + k*0x30
        print(' k7', k, s.r(a, 0x30).hex(' '))
