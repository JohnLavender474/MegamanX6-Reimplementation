from common import *
s = R(sys.argv[1])
print([hex(s.w(0x8007136C+4*i)) for i in range(5)])
for k in range(4):
    a = 0x800C4778 + k*0x30
    print(k, hex(s.w(a+0x10)), s.r(s.w(a+0x10), 8).hex(' ') if s.w(a+0x10) else '')
