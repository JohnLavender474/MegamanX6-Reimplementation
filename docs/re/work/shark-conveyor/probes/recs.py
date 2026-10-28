from common import *
s = R(sys.argv[1]); a = int(sys.argv[2], 16)
for i in range(200):
    r = s.r(a+8*i, 8)
    if r[3] == 0xff or r == b'\xff'*8: print('end', i, r.hex(' ')); break
    print(i, f'{a+8*i:08x}', r.hex(' '), 'kind', r[3], 'type', r[1], 'id', r[2], 'x', struct.unpack_from('<h', r, 4)[0], 'y', struct.unpack_from('<h', r, 6)[0])
