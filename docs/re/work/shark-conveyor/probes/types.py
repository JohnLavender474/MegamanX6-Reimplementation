from common import *
from mmxdemake.savestate import read_state
from mmxdemake.sprites import scratchpad
st = read_state(X/'dumps'/sys.argv[1]); sp = scratchpad(st); s = R(sys.argv[1])
for off, n, name in ((0x80, 77, 'main'), (0x84, 58, 'visual'), (0x90, 64, 'misc'), (0x94, 32, 'stage'), (0x8C, 45, 'logic')):
    t = struct.unpack_from('<I', sp, off)[0]
    print(name, ' '.join(f'{i}:{s.w(t+4*i):08x}' for i in range(n) if 0x800E9860 <= s.w(t+4*i) < 0x800F8000))
