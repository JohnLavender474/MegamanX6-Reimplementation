import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
ranges = [(0x80010000, 0x80070000), (0x800E9860, 0x800FA000), (0x801EA000, 0x801F6000)]
for lo, hi in ranges:
    for a in range(lo, hi, 4):
        word = w(ram, a); op = word >> 26; imm = word & 0xFFFF
        if op in (0x28, 0x29, 0x20, 0x24, 0x21, 0x25) and imm in (0xD3D6, 0xD3D7):
            print('ABS', dis(ram, a, 1))
        if op in (0x28, 0x29) and imm in (150, 151):
            print('---'); print(dis(ram, a - 16, 6))
