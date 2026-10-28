import sys
sys.path.insert(0, '.')
from sp import *
ram = load(sys.argv[1])
targets = [int(t, 16) for t in sys.argv[2:]]
regs = {}
for a in range(0x80010000, 0x80070000, 4):
    word = w(ram, a); op = word >> 26
    if op == 0x0F:
        regs[(word >> 16) & 31] = ((word & 0xFFFF) << 16, a)
    elif op in (0x09, 0x23, 0x21, 0x25, 0x20, 0x24):
        rs = (word >> 21) & 31; imm = word & 0xFFFF; imm -= 0x10000 if imm & 0x8000 else 0
        if rs in regs and a - regs[rs][1] < 64:
            v = (regs[rs][0] + imm) & 0xFFFFFFFF
            for t in targets:
                if t - 0x10 <= v <= t + 0x10:
                    print(f'{a:08x} -> {v:08x}  {dis(ram,a,1)}')
