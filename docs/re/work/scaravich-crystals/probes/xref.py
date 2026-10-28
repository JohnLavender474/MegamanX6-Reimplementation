import sys
from ramdis import *
from mmxdemake.mips import disassemble
r = ram(sys.argv[1])
lo, hi = 0x80010000, 0x80100000
jal_targets = {0x8002D2D4: 'GetRNG', 0x8002D30C: 'GetRNG1'}
calls = {k: [] for k in jal_targets}
lui = {}
hits = []
for a in range(lo, hi, 4):
    w = u32(r, a)
    op = w >> 26
    if op == 3:
        t = (a & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
        if t in calls: calls[t].append(a)
    if op == 0x0F:
        lui[(w >> 16) & 31] = (w & 0xFFFF, a)
    if op in (0x20, 0x21, 0x24, 0x25, 0x28, 0x29, 0x23, 0x2B, 0x09):
        rs = (w >> 21) & 31; imm = w & 0xFFFF
        imm = imm - 0x10000 if imm & 0x8000 else imm
        if rs in lui and a - lui[rs][1] < 64:
            full = ((lui[rs][0] << 16) + imm) & 0xFFFFFFFF
            if full in (0x80090E70, 0x800CCF00, 0x800CCF01, 0x800CCEFF):
                hits.append((a, full, disassemble(w, a)))
for t, l in calls.items(): print(jal_targets[t], len(l), ' '.join(hex(x) for x in l))
for h in hits: print(hex(h[0]), hex(h[1]), h[2])
