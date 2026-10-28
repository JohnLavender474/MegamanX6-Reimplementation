from common import *
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
name = sys.argv[1]; st = read_state(X/'dumps'/name); sp = scratchpad(st); s = R(name)
spw = lambda o: struct.unpack_from('<I', sp, o)[0]
lay, scr, tdef = spw(4), spw(8), spw(0xC); W = s.b(0x800CD338)
def tile(x, y):
    sc = s.b(lay + W*(y>>8) + (x>>8))
    t = s.H(scr + sc*512 + ((y>>4)&15)*32 + ((x>>4)&15)*2) & 0x3fff
    return t, s.r(tdef+4*t, 4).hex()
for y in (int(a) for a in sys.argv[3].split(',')):
    print(y, [tile(x, y)[0] for x in range(int(sys.argv[2].split('-')[0]), int(sys.argv[2].split('-')[1]), 16)])
print('defs', {t: s.r(tdef+4*t,4).hex() for t in (56,57,58,59,208,209,214,215,226,227,234,235)})
