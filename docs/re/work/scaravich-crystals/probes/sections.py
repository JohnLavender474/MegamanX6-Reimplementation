import sys, struct
from ramdis import ram, u32, u16, u8
from mmxdemake.mips import disassemble
r = ram('scaravich-1_start.state.gz')
jt = [u32(r, 0x800E98CC + 4 * i) for i in range(8)]
end = 0x800EBE7C
bounds = sorted(jt) + [end]
out = {}
for s, start in enumerate(jt):
    stop = min(b for b in bounds if b > start)
    rolls, regs = [], {}
    for a in range(start, stop, 4):
        w = u32(r, a); op = w >> 26; rt = (w >> 16) & 31; imm = w & 0xFFFF
        if op == 9: regs[rt] = (struct.unpack('<h', struct.pack('<H', imm))[0], regs.get((w >> 21) & 31))
        if op == 0x0F: regs[rt] = ('lui', imm)
        if op == 3:
            t = (w & 0x3FFFFFF) << 2 | 0x80000000
            if t == 0x8002D2D4: rolls.append(['rng', a])
            if t == 0x80027BA0:
                w2 = u32(r, a + 4)
                src = 0x800F0000 + struct.unpack('<h', struct.pack('<H', w2 & 0xFFFF))[0]
                rolls[-1] += [regs[5][0], regs[6][0], src, struct.unpack('<4h', bytes(r[src & 0x1FFFFF:(src & 0x1FFFFF) + 8]))]
    out[s] = rolls
    print('section', s, hex(start), len(rolls), 'rolls')
    for x in rolls: print('   ', hex(x[1]), *x[2:])
