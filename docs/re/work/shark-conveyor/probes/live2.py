from common import *
from mmxdemake.savestate import read_state, ram_from
from mmxdemake.sprites import scratchpad
st = read_state(X/'dumps'/sys.argv[1]); sp = scratchpad(st)
s = R(sys.argv[1])
lt = struct.unpack_from('<I', sp, 0x8C)[0]; stt = struct.unpack_from('<I', sp, 0x94)[0]
print('logic table', hex(lt), 'stage table', hex(stt))
for i in range(80):
    a = 0x800C1220 + i*0xA4; o = s.r(a, 0xA4)
    if o[0]: print(f'stage {i:2} type {o[1]} var {o[2]} st {o[4:7].hex(" ")} x {s.h(a+0xA)} y {s.h(a+0xE)} entry {s.w(stt+4*o[1]):08x} +30 {s.w(a+0x30):08x} +68 {o[0x68:0x6c].hex(" ")} {o[:0x20].hex(" ")}')
for i in range(96):
    a = 0x8009B7B0 + i*0x30; o = s.r(a, 0x30)
    if o[0]: print(f'logic {i:2} type {o[1]} entry {s.w(lt+4*o[1]):08x} {o.hex(" ")}')
