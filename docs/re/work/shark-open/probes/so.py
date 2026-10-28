import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
typ = int(sys.argv[2])
for i in range(80):
    a = 0x800C1220 + i*0xA4
    if b(ram, a) and b(ram, a+1) == typ:
        print(f'{a:08x} hdr {ram[a&0x1fffff:(a&0x1fffff)+8].hex()} x {h(ram,a+0xA)} y {h(ram,a+0xE)} atk {w(ram,a+0x50):08x} hurt {w(ram,a+0x54):08x} terr {w(ram,a+0x68):08x} hp {b(ram,a+0x5C)} dmg {w(ram,a+0x58):08x} 61 {b(ram,a+0x61)}')
